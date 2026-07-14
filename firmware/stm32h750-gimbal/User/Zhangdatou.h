#ifndef __ZHANGDATOU_H
#define __ZHANGDATOU_H
#include "main.h"
/* 张大头驱动器接口：X 对应 UART2，Y 对应 UART3。state 为 0 失能、非 0 使能。 */
void zhang_motor_enable_x(uint8_t state);
void zhang_motor_enable_y(uint8_t state);

/* 注册两个驱动器 UART 的接收回调，并启动首次单字节接收。 */
void zhang_motor_init();

/* 同时设置两台驱动器的电流设定值；单位和允许范围以驱动器协议为准。 */
void zhang_motor_current(uint16_t current);

/* 将两台驱动器设为 256 微步。调用前应确认机构允许该细分设置。 */
void zhang_motor_256_microsteps();

/* 以下四个函数切换 X/Y 轴的开环或闭环模式。 */
void zhang_y_zero();
void zhang_set_zero();

void zhang_motor_openloop_x();
void zhang_motor_closeloop_x();
void zhang_motor_openloop_y();
void zhang_motor_closeloop_y();
/* 请求 Y 轴返回当前角度/编码器值；结果异步保存到 Zhangdatou.c 的 Mencounder。 */
void zhang_get_y_angle();
#endif
