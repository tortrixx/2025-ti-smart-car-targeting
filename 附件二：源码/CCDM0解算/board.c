 /*
 * This file is the serial port configuration file and clock configuration file.
 * forum: https://oshwhub.com/forum
 * Change Logs:
 * Date           Author       Notes
 * 2024-06-26     LCKFB     first version
 */
 
#include "board.h"
#include <stdio.h>

#define RE_0_BUFF_LEN_MAX	128

volatile uint8_t  recv0_buff[RE_0_BUFF_LEN_MAX] = {0};
volatile uint16_t recv0_length = 0;
volatile uint8_t  recv0_flag = 0;

void board_init(void)
{
	// SYSCFG初始化
	SYSCFG_DL_init();	
}

//搭配滴答定时器实现的精确us延时
void delay_us (unsigned long __us) 
{
	while(__us --)
		DL_Common_delayCycles(32);
    
}
//搭配滴答定时器实现的精确ms延时
void delay_ms(unsigned long ms) 
{
	delay_us( ms * 1000 );
}

void delay_1us(unsigned long __us){ delay_us(__us); }
void delay_1ms(unsigned long ms){ delay_ms(ms); }


