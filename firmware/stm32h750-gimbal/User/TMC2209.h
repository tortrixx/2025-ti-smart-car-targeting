#ifndef __TMC_2209_H
#define __TMC_2209_H
#include "main.h"

#define SYNC 0x05       /* TMC2209 UART 数据帧同步字节 */
#define ADDR 0x00       /* 默认从机地址 */
#define GCONF 0X00      /* 全局配置寄存器地址 */
#define CHOPCONF 0X6C   /* 斩波器/微步相关配置寄存器地址 */

/* 配置两个 TMC2209。main.c 当前未调用，启用前需确认 UART2/UART3 未被其他驱动器占用。 */
void tms2209_init();

#endif
