/*
 * empty.c - MSPM0G3507 自动循迹小车主程序
 * empty.c - MSPM0G3507 Line-Following Car Main
 *
 * 2025年电赛基础部分: 正方形环形黑线自动循迹
 * 2025 Competition Basic Task: Auto track square loop black line
 *
 * 参考已验证项目校准 / Calibrated against proven reference project
 *
 * 硬件 / Hardware:
 *   - MSPM0G3507 LaunchPad
 *   - 8路GPIO数字灰度传感器 (X1~X8)
 *   - AT8236双路电机驱动
 *
 * 架构 / Architecture:
 *   - TIMG0/TIMG7: hardware PWM updates motor duty continuously
 *   - every 10ms: LineFollower_Task() executes line tracking
 *
 * 模式选择 / Mode Select:
 *   #define FORCE_MOTOR_STOP_TEST 1    → 仅停止电机 (安全验证)
 *   #define SENSOR_RAW_DEBUG 1         → 仅调试传感器原始值
 *   #define TRACKING_ALGORITHM_DEBUG 1 → 完整循迹 + 电机强制关闭 (debugger观察)
 *   都不定义                           → 正常循迹模式
 */

#define FORCE_MOTOR_STOP_TEST       0
#define FORCE_STRAIGHT_FORWARD_TEST 0
#define SENSOR_RAW_DEBUG            0
#define TRACKING_ALGORITHM_DEBUG    0

#include <stdint.h>

#include "ti_msp_dl_config.h"

#include "line_follow.h"
#include "line_sensor.h"
#include "motor.h"

#if SENSOR_RAW_DEBUG
static uint8_t count_raw_ones(uint8_t raw)
{
    uint8_t count = 0U;
    uint8_t i;

    for (i = 0U; i < 8U; i++) {
        if (((raw >> i) & 0x01U) != 0U) {
            count++;
        }
    }

    return count;
}
#endif

static void simple_delay_ms(uint32_t ms)
{
    volatile uint32_t i;
    while (ms--) {
        for (i = 0U; i < 4000U; i++) {
            __asm volatile ("nop");
        }
    }
}

static volatile uint32_t g_control_tick_count;
static uint32_t g_control_last_tick;

void SysTick_Handler(void)
{
    g_control_tick_count++;
}

static void control_scheduler_reset(void)
{
    g_control_last_tick = g_control_tick_count;
}

static void wait_for_control_tick(void)
{
    uint32_t tick;

    while (1) {
        tick = g_control_tick_count;
        if (tick != g_control_last_tick) {
            g_control_last_tick = tick;
            return;
        }
        __WFI();
    }
}

int main(void)
{
    SYSCFG_DL_init();
    simple_delay_ms(500);
    Motor_Init();
    control_scheduler_reset();

#if FORCE_MOTOR_STOP_TEST
    Motor_Stop();
    while (1) {
        __WFI();
    }

#elif FORCE_STRAIGHT_FORWARD_TEST
    /*
     * 强制直行 (验证电机方向和左右一致性)
     */
    LineSensor_Init();
    while (1) {
        wait_for_control_tick();
        Motor_SetSpeeds(120, 120);
    }

#elif SENSOR_RAW_DEBUG
    /*
     * 传感器原始值调试
     *
     * CCS Expressions 观察:
     *   g_debug_line_raw   → 原始8位值
     *   g_debug_line_count → 高电平位数
     *   g_debug_line_ok    → 读取成功标志
     *   LineSensor_GetWhitePattern() → 校准白值
     *   LineSensor_IsCalibrated()    → 校准是否通过
     */
    {
        volatile uint8_t g_debug_line_raw;
        volatile uint8_t g_debug_line_count;
        volatile bool g_debug_line_ok;
        uint8_t sample_divider = 0U;

        LineSensor_Init();
        Motor_Stop();

        while (1) {
            wait_for_control_tick();
            sample_divider++;
            if (sample_divider >= 2U) {
                sample_divider = 0U;
                g_debug_line_ok = LineSensor_ReadRaw(&g_debug_line_raw);
                if (g_debug_line_ok) {
                    g_debug_line_count = count_raw_ones(g_debug_line_raw);
                }
            }
        }
    }

#elif TRACKING_ALGORITHM_DEBUG
    /*
     * 循迹算法调试 (电机强制关闭)
     *
     * CCS Expressions 观察:
     *   g_debug_error         → 位置误差 (-3500 ~ +3500)
     *   g_debug_black_count   → 检测到黑线的传感器数量
     *   g_debug_sensor_bits   → 传感器原始位值
     *   g_debug_follower_mode → 状态机 (0=WAIT,1=TRACK,2=BRIDGE, 0xFF=校准失败)
     *   g_debug_left_speed    → 左轮目标速度
     *   g_debug_right_speed   → 右轮目标速度
     *   g_debug_turn          → 转向量
     */
    LineSensor_Init();

    while (1) {
        wait_for_control_tick();
        LineFollower_Task();
        Motor_Stop();
    }

#else
    /*
     * 正常循迹模式
     *
     * 启动流程:
     *   1. 上电 → 校准传感器 (车子必须停在白色背景上!)
     *   2. 校准失败 → 电机保持停止 (g_debug_follower_mode = 0xFF)
     *   3. 校准成功 → WAIT_LINE 等待黑线
     *   4. 检测到黑线 → TRACK_LINE 循迹
     *
     * 调参: 只改 app_config.h
     */
    LineSensor_Init();

    while (1) {
        wait_for_control_tick();
        LineFollower_Task();
    }
#endif
}
