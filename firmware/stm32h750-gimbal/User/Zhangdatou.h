#ifndef __ZHANGDATOU_H
#define __ZHANGDATOU_H
#include "main.h"
void zhang_motor_enable_x(uint8_t state);
void zhang_motor_enable_y(uint8_t state);
void zhang_motor_init();

void zhang_motor_current(uint16_t current);

void zhang_motor_256_microsteps();
void zhang_y_zero();
void zhang_set_zero();

void zhang_motor_openloop_x();
void zhang_motor_closeloop_x();
void zhang_motor_openloop_y();
void zhang_motor_closeloop_y();
void zhang_get_y_angle();
#endif