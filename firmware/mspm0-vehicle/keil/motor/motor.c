#include "motor.h"

#include <stdbool.h>

#include "app_config.h"
#include "ti_msp_dl_config.h"

/*
 * motor.c - AT8236 双路电机 软件PWM
 * motor.c - AT8236 dual motor software PWM
 *
 * PWM原理:
 *   周期 = 20步, 每1ms执行一次 Motor_UpdateSoftwarePwm()
 *   占空比 = duty / CAR_PWM_MAX * 20步
 *
 * PWM principle:
 *   Cycle = 20 steps, Motor_UpdateSoftwarePwm() every 1ms
 *   Active steps = duty / CAR_PWM_MAX * 20
 */

#define MOTOR_SOFT_PWM_STEPS       (20U)

static int16_t g_left_speed;
static int16_t g_right_speed;
static uint8_t g_pwm_phase;
static uint16_t g_left_active_steps;
static uint16_t g_right_active_steps;
static uint16_t g_left_pwm_remainder;
static uint16_t g_right_pwm_remainder;

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

static void motor_gpio_init(void)
{
    DL_GPIO_initDigitalOutputFeatures(GPIO_PWM_LEFT_C0_IOMUX,
        DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_NONE,
        DL_GPIO_DRIVE_STRENGTH_HIGH, DL_GPIO_HIZ_DISABLE);
    DL_GPIO_initDigitalOutputFeatures(GPIO_PWM_LEFT_C1_IOMUX,
        DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_NONE,
        DL_GPIO_DRIVE_STRENGTH_HIGH, DL_GPIO_HIZ_DISABLE);
    DL_GPIO_initDigitalOutputFeatures(GPIO_PWM_RIGHT_C0_IOMUX,
        DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_NONE,
        DL_GPIO_DRIVE_STRENGTH_HIGH, DL_GPIO_HIZ_DISABLE);
    DL_GPIO_initDigitalOutputFeatures(GPIO_PWM_RIGHT_C1_IOMUX,
        DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_NONE,
        DL_GPIO_DRIVE_STRENGTH_HIGH, DL_GPIO_HIZ_DISABLE);

    DL_GPIO_enableOutput(GPIOA,
        GPIO_PWM_LEFT_C0_PIN | GPIO_PWM_LEFT_C1_PIN |
        GPIO_PWM_RIGHT_C0_PIN | GPIO_PWM_RIGHT_C1_PIN);
}

static void write_motor_gpio(uint32_t pin_forward, uint32_t pin_reverse,
    int16_t speed, uint16_t active_steps, bool invert)
{
    uint16_t duty;

    if (invert) {
        speed = (int16_t)-speed;
    }

    duty = clamp_pwm(speed);

    DL_GPIO_clearPins(GPIOA, pin_forward | pin_reverse);

    if ((duty == 0U) || (g_pwm_phase >= active_steps)) {
        return;
    }

    if (speed > 0) {
        DL_GPIO_setPins(GPIOA, pin_forward);
    } else {
        DL_GPIO_setPins(GPIOA, pin_reverse);
    }
}

void Motor_Init(void)
{
    motor_gpio_init();
    Motor_Stop();
}

void Motor_SetLeft(int16_t speed)
{
    g_left_speed = speed;
}

void Motor_SetRight(int16_t speed)
{
    g_right_speed = speed;
}

void Motor_SetSpeeds(int16_t left_speed, int16_t right_speed)
{
    Motor_SetLeft(left_speed);
    Motor_SetRight(right_speed);
}

void Motor_UpdateSoftwarePwm(void)
{
    if (g_pwm_phase == 0U) {
        uint32_t scaled_steps;

        scaled_steps = (uint32_t)clamp_pwm(g_left_speed) *
            MOTOR_SOFT_PWM_STEPS;
        g_left_active_steps = (uint16_t)(scaled_steps / CAR_PWM_MAX);
        g_left_pwm_remainder = (uint16_t)(g_left_pwm_remainder +
            (uint16_t)(scaled_steps % CAR_PWM_MAX));
        if (g_left_pwm_remainder >= CAR_PWM_MAX) {
            g_left_pwm_remainder = (uint16_t)(g_left_pwm_remainder -
                CAR_PWM_MAX);
            g_left_active_steps++;
        }

        scaled_steps = (uint32_t)clamp_pwm(g_right_speed) *
            MOTOR_SOFT_PWM_STEPS;
        g_right_active_steps = (uint16_t)(scaled_steps / CAR_PWM_MAX);
        g_right_pwm_remainder = (uint16_t)(g_right_pwm_remainder +
            (uint16_t)(scaled_steps % CAR_PWM_MAX));
        if (g_right_pwm_remainder >= CAR_PWM_MAX) {
            g_right_pwm_remainder = (uint16_t)(g_right_pwm_remainder -
                CAR_PWM_MAX);
            g_right_active_steps++;
        }
    }

    write_motor_gpio(GPIO_PWM_LEFT_C0_PIN, GPIO_PWM_LEFT_C1_PIN,
        g_left_speed, g_left_active_steps, MOTOR_LEFT_INVERT != 0);
    write_motor_gpio(GPIO_PWM_RIGHT_C0_PIN, GPIO_PWM_RIGHT_C1_PIN,
        g_right_speed, g_right_active_steps, MOTOR_RIGHT_INVERT != 0);

    g_pwm_phase++;
    if (g_pwm_phase >= MOTOR_SOFT_PWM_STEPS) {
        g_pwm_phase = 0U;
    }
}

void Motor_Stop(void)
{
    Motor_SetSpeeds(0, 0);
    g_left_active_steps  = 0U;
    g_right_active_steps = 0U;
    g_left_pwm_remainder  = 0U;
    g_right_pwm_remainder = 0U;
    DL_GPIO_clearPins(GPIOA,
        GPIO_PWM_LEFT_C0_PIN | GPIO_PWM_LEFT_C1_PIN |
        GPIO_PWM_RIGHT_C0_PIN | GPIO_PWM_RIGHT_C1_PIN);
}
