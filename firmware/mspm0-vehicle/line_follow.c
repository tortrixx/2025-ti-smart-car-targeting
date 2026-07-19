#include "line_follow.h"

#include <stdbool.h>
#include <stdint.h>

#include "app_config.h"
#include "line_sensor.h"
#include "motor.h"

/*
 * line_follow.c - 循迹状态机 + PD差速控制
 * line_follow.c - Line-following state machine + PD diff control
 *
 * 2025电赛基础部分: 正方形环形黑线循迹
 * 2025 basic task: square loop black line tracking
 *
 * 参考已验证项目参数校准 / Calibrated against proven reference project
 *
 * 状态机 / State machine:
 *   WAIT_LINE       → 等待黑线出现
 *   TRACK_LINE      → PD差速循迹
 *   BRIDGE_STRAIGHT → 丢线恢复（保持转向惯性直行）
 */

typedef enum {
    FOLLOWER_WAIT_LINE = 0,
    FOLLOWER_TRACK_LINE,
    FOLLOWER_BRIDGE_STRAIGHT,
} Follower_Mode;

static Follower_Mode g_mode;
static int16_t g_last_error;
static int16_t g_last_turn;
static int16_t g_last_turn_on_loss;
static uint16_t g_bridge_ticks;
static uint8_t g_reacquire_ticks;
static bool g_cal_was_ok;

int16_t g_debug_error = 0;
uint8_t g_debug_black_count = 0;
uint8_t g_debug_sensor_bits = 0;
uint8_t g_debug_follower_mode = 0;
int16_t g_debug_left_speed = 0;
int16_t g_debug_right_speed = 0;
int16_t g_debug_turn = 0;

static int16_t clamp_i16(int16_t value, int16_t min_value, int16_t max_value)
{
    if (value < min_value) return min_value;
    if (value > max_value) return max_value;
    return value;
}

static int16_t abs_i16(int16_t value)
{
    return (value < 0) ? (int16_t)-value : value;
}

static bool sensor_is_black(const LineSensor_Data *sensor, uint8_t index)
{
    return sensor->value[index] != 0U;
}

static bool line_is_valid(uint8_t black_count)
{
    return (black_count > 0U) && (black_count < CAR_SOLID_BLACK_COUNT);
}

static void set_forward_speeds(int16_t left_speed, int16_t right_speed)
{
    left_speed = clamp_i16(left_speed, 0, CAR_PWM_MAX);
    right_speed = clamp_i16(right_speed, 0, CAR_PWM_MAX);
    g_debug_left_speed  = left_speed;
    g_debug_right_speed = right_speed;
    Motor_SetSpeeds(left_speed, right_speed);
}

static void set_diff_speeds(int16_t base_speed, int16_t turn)
{
    int16_t max_turn = (int16_t)(base_speed - CAR_MIN_INNER_SPEED);

    if (max_turn < 0) {
        max_turn = 0;
    }

    turn = clamp_i16(turn, (int16_t)-max_turn, max_turn);
    set_forward_speeds((int16_t)(base_speed + turn),
                       (int16_t)(base_speed - turn));
}

static int16_t calc_line_error(const LineSensor_Data *sensor,
    uint8_t *black_count)
{
    static const int16_t weights[LINE_SENSOR_COUNT] = {
        -3500, -2500, -1500, -500, 500, 1500, 2500, 3500
    };
    int32_t sum = 0;
    uint8_t count = 0U;
    uint8_t i;

    for (i = 0U; i < LINE_SENSOR_COUNT; i++) {
        if (sensor_is_black(sensor, i)) {
            sum += weights[i];
            count++;
        }
    }

    *black_count = count;
    if (count == 0U) {
        return 0;
    }

    return (int16_t)(sum / (int32_t)count);
}

static int16_t calc_base_speed(int16_t error, uint8_t black_count)
{
    int16_t speed = (int16_t)(CAR_BASE_SPEED -
        (abs_i16(error) / CAR_SPEED_SLOWDOWN_DIV));

    if (black_count > 2U) {
        speed = (int16_t)(speed -
            ((int16_t)(black_count - 2U) * CAR_MULTI_LINE_SLOWDOWN));
    }

    return clamp_i16(speed, CAR_MIN_TURN_SPEED, CAR_BASE_SPEED);
}

static int16_t limit_turn_step(int16_t turn)
{
    int16_t min_turn = (int16_t)(g_last_turn - CAR_TURN_STEP_LIMIT);
    int16_t max_turn = (int16_t)(g_last_turn + CAR_TURN_STEP_LIMIT);

    turn = clamp_i16(turn, min_turn, max_turn);
    g_last_turn = turn;
    return turn;
}

static int16_t calc_turn_delta(int16_t error)
{
    int16_t derivative;
    int32_t turn;

    if (abs_i16(error) < CAR_TURN_DEADBAND) {
        error = 0;
    }

    derivative = (int16_t)(error - g_last_error);
    turn = ((int32_t)error * CAR_POSITION_KP) +
           ((int32_t)derivative * CAR_POSITION_KD);
    turn /= CAR_TURN_SCALE;

    if ((error != 0) && (abs_i16(error) < CAR_FINE_TURN_ERROR)) {
        turn /= 2;
    }

    turn = clamp_i16((int16_t)turn, -CAR_MAX_TURN_DELTA, CAR_MAX_TURN_DELTA);
    turn = limit_turn_step((int16_t)turn);

    g_last_error = error;
    g_debug_turn = (int16_t)turn;

    return (int16_t)turn;
}

static void reset_control_history(int16_t error)
{
    g_last_error = error;
    g_last_turn = 0;
}

static void enter_track(int16_t error)
{
    g_mode = FOLLOWER_TRACK_LINE;
    g_bridge_ticks = 0U;
    g_reacquire_ticks = 0U;
    reset_control_history(error);
}

static void enter_bridge(void)
{
    g_mode = FOLLOWER_BRIDGE_STRAIGHT;
    g_bridge_ticks = 0U;
    g_reacquire_ticks = 0U;
    reset_control_history(0);
}

static void drive_track(int16_t error, uint8_t black_count)
{
    int16_t base_speed;
    int16_t turn;

    if (!line_is_valid(black_count)) {
#if CAR_BRIDGE_ENABLE
        g_last_turn_on_loss = g_debug_turn;
        enter_bridge();
        set_forward_speeds(CAR_BRIDGE_SPEED, CAR_BRIDGE_SPEED);
#else
        Motor_Stop();
#endif
        return;
    }

    base_speed = calc_base_speed(error, black_count);
    turn = calc_turn_delta(error);
    set_diff_speeds(base_speed, turn);
}

static void drive_bridge(int16_t error, uint8_t black_count)
{
    int16_t bridge_turn;

    if (g_bridge_ticks < 0xFFFFU) {
        g_bridge_ticks++;
    }

    /*
     * 丢线初期保持最后转向的50%, 帮助完成直角弯
     * 之后纯直行搜索线
     */
    if (g_bridge_ticks < CAR_BRIDGE_MIN_STRAIGHT_TICKS) {
        bridge_turn = (int16_t)(g_last_turn_on_loss / 2);
    } else {
        bridge_turn = 0;
    }
    set_diff_speeds(CAR_BRIDGE_SPEED, bridge_turn);

    /* 重新检测到线 → 回到循迹 */
    if ((g_bridge_ticks >= CAR_BRIDGE_MIN_STRAIGHT_TICKS) &&
        line_is_valid(black_count)) {
        if (g_reacquire_ticks < CAR_LINE_REACQUIRE_TICKS) {
            g_reacquire_ticks++;
        }
        if (g_reacquire_ticks >= CAR_LINE_REACQUIRE_TICKS) {
            enter_track(error);
            drive_track(error, black_count);
            return;
        }
    } else {
        g_reacquire_ticks = 0U;
    }

    /* 超时 → 停车等待 */
    if (g_bridge_ticks >= CAR_BRIDGE_MAX_TICKS) {
        g_mode = FOLLOWER_WAIT_LINE;
        g_bridge_ticks = 0U;
        Motor_Stop();
    }
}

void LineFollower_Reset(void)
{
    g_mode = FOLLOWER_WAIT_LINE;
    g_last_error = 0;
    g_last_turn = 0;
    g_last_turn_on_loss = 0;
    g_bridge_ticks = 0U;
    g_reacquire_ticks = 0U;
    g_cal_was_ok = false;

    g_debug_error = 0;
    g_debug_black_count = 0;
    g_debug_sensor_bits = 0;
    g_debug_follower_mode = 0;
    g_debug_left_speed = 0;
    g_debug_right_speed = 0;
    g_debug_turn = 0;
}

bool LineFollower_Task(void)
{
    LineSensor_Data sensor;
    uint8_t black_count;
    int16_t error;

    if (!g_cal_was_ok) {
        if (!LineSensor_IsCalibrated()) {
            g_debug_follower_mode = 0xFFU;
            Motor_Stop();
            return false;
        }
        g_cal_was_ok = true;
    }

    if (!LineSensor_Read(&sensor)) {
        Motor_Stop();
        return false;
    }

    error = calc_line_error(&sensor, &black_count);

    g_debug_error = error;
    g_debug_black_count = black_count;
    g_debug_sensor_bits = sensor.raw;
    g_debug_follower_mode = (uint8_t)g_mode;

    switch (g_mode) {
    case FOLLOWER_WAIT_LINE:
        if (line_is_valid(black_count)) {
            enter_track(error);
            drive_track(error, black_count);
        } else {
            Motor_Stop();
        }
        break;

    case FOLLOWER_TRACK_LINE:
        drive_track(error, black_count);
        break;

    case FOLLOWER_BRIDGE_STRAIGHT:
        drive_bridge(error, black_count);
        break;

    default:
        Motor_Stop();
        break;
    }

    return true;
}
