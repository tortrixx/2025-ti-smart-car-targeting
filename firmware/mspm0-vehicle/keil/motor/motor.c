#include "motor.h"

#include <stdbool.h>

#include "app_config.h"
#include "ti_msp_dl_config.h"

/*
 * motor.c - AT8236 双路电机 软件PWM
 * motor.c - AT8236 dual motor hardware PWM
 *
 * PWM原理:
 *   周期 = 20步, 每1ms执行一次 Motor_UpdateSoftwarePwm()
 *   占空比 = duty / CAR_PWM_MAX * 20步
 *
 * PWM principle:
 *   TIMG0/TIMG7 generate both H-bridge inputs in hardware.
 *   One channel is driven with PWM while the opposite channel is held LOW.
 */

#define MOTOR_PWM_PERIOD_COUNTS    ((uint16_t)CAR_PWM_MAX)
#define MOTOR_PWM_COMPARE_OFF      (MOTOR_PWM_PERIOD_COUNTS - 1U)

static uint16_t clamp_pwm(int16_t speed)
{
    if (speed < 0) {
        speed = (int16_t)-speed;
    }
    if (speed > CAR_PWM_MAX) {
        speed = CAR_PWM_MAX;
    }
    return (uint16_t)speed;
}

static uint16_t duty_to_compare(uint16_t duty)
{
    if (duty >= MOTOR_PWM_PERIOD_COUNTS) {
        return 0U;
    }

    return (uint16_t)(MOTOR_PWM_COMPARE_OFF - duty);
}

static void write_motor_pwm(GPTIMER_Regs *timer, DL_TIMER_CC_INDEX forward_cc,
    DL_TIMER_CC_INDEX reverse_cc, int16_t speed, bool invert)
{
    uint16_t duty;
    DL_TIMER_CC_INDEX active_cc;
    DL_TIMER_CC_INDEX idle_cc;

    if (invert) {
        speed = (int16_t)-speed;
    }

    duty = clamp_pwm(speed);
    active_cc = (speed >= 0) ? forward_cc : reverse_cc;
    idle_cc = (speed >= 0) ? reverse_cc : forward_cc;

    DL_TimerG_setCaptureCompareValue(timer, MOTOR_PWM_COMPARE_OFF, idle_cc);
    DL_TimerG_setCaptureCompareValue(timer, duty_to_compare(duty), active_cc);
}

void Motor_Init(void)
{
    DL_TimerG_setCaptCompUpdateMethod(PWM_LEFT_INST,
        DL_TIMER_CC_UPDATE_METHOD_ZERO_EVT, GPIO_PWM_LEFT_C0_IDX);
    DL_TimerG_setCaptCompUpdateMethod(PWM_LEFT_INST,
        DL_TIMER_CC_UPDATE_METHOD_ZERO_EVT, GPIO_PWM_LEFT_C1_IDX);
    DL_TimerG_setCaptCompUpdateMethod(PWM_RIGHT_INST,
        DL_TIMER_CC_UPDATE_METHOD_ZERO_EVT, GPIO_PWM_RIGHT_C0_IDX);
    DL_TimerG_setCaptCompUpdateMethod(PWM_RIGHT_INST,
        DL_TIMER_CC_UPDATE_METHOD_ZERO_EVT, GPIO_PWM_RIGHT_C1_IDX);

    Motor_Stop();
    DL_TimerG_startCounter(PWM_LEFT_INST);
    DL_TimerG_startCounter(PWM_RIGHT_INST);
}

void Motor_SetLeft(int16_t speed)
{
    write_motor_pwm(PWM_LEFT_INST, GPIO_PWM_LEFT_C0_IDX, GPIO_PWM_LEFT_C1_IDX,
        speed, MOTOR_LEFT_INVERT != 0);
}

void Motor_SetRight(int16_t speed)
{
    write_motor_pwm(PWM_RIGHT_INST, GPIO_PWM_RIGHT_C0_IDX,
        GPIO_PWM_RIGHT_C1_IDX, speed, MOTOR_RIGHT_INVERT != 0);
}

void Motor_SetSpeeds(int16_t left_speed, int16_t right_speed)
{
    Motor_SetLeft(left_speed);
    Motor_SetRight(right_speed);
}

void Motor_Stop(void)
{
    Motor_SetSpeeds(0, 0);
}
