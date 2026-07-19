#ifndef MOTOR_H_
#define MOTOR_H_

#include <stdint.h>

/*
 * motor.h - AT8236 双路电机驱动 (软件PWM)
 * motor.h - AT8236 dual motor driver (software PWM)
 *
 * 硬件连接 / Hardware:
 *   左电机: AT8236 AIN1=PA12, AIN2=PA13 → TIMG0
 *   右电机: AT8236 BIN1=PA28, BIN2=PA31 → TIMG7
 *
 * PWM方式 / PWM method:
 *   软件PWM: 20步/周期, 每1ms更新一次, PWM频率≈50Hz
 *   Software PWM: 20 steps/cycle, update every 1ms, PWM freq≈50Hz
 */

void Motor_Init(void);
void Motor_SetLeft(int16_t speed);
void Motor_SetRight(int16_t speed);
void Motor_SetSpeeds(int16_t left_speed, int16_t right_speed);
void Motor_UpdateSoftwarePwm(void);
void Motor_Stop(void);

#endif /* MOTOR_H_ */
