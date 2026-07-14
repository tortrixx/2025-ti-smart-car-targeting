/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32h7xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
/*
 * PID 控制器运行时状态。
 *
 * 本项目的 pid_calculate() 将 P、I、D 项按实际时间间隔累加到 OUT；因此 OUT
 * 是“累计后的脉冲目标”，而不是一次计算直接得到的 PWM 占空比。调参时请结合
 * 调用周期一起看，不能简单套用普通固定周期 PID 的参数。
 */
typedef struct pid_handler{
	float P;                  /* 比例系数：当前误差越大，响应越强 */
	float I;                  /* 一次积分系数：消除长期静差 */
  float I2;                 /* 二次积分系数；当前主控制器未使用 */
	float D;                  /* 微分系数：根据误差变化速度抑制振荡 */
	float integral;           /* 一次积分的累积误差 */
  float integral2;          /* 二次积分的累积值 */
	float previous;           /* 上一次误差，用于计算普通微分 */
  float de;                 /* 外部提供的误差变化率（d_state 非 0 时使用） */
	float out_limit[2];       /* OUT 的下限、上限；主函数目前未启用该限幅 */
  float inte_limit[2];      /* integral 的下限、上限，防止积分饱和 */
  float OUT;                /* 累积输出：供步进脉冲插补使用 */
  float last_previous;      /* 上上次误差，仅供增量式 PID 使用 */
  float err_limit;          /* 预留的误差阈值；当前主 PID 未启用 */
  float acc_Feed;           /* 预留的加速度前馈量；当前主 PID 未启用 */
  uint8_t d_state;          /* 0：内部差分；非 0：使用 de */
  uint32_t last_time;       /* 上次运算时间，单位：微秒 */
	
}pid_handler;

/* 将 in 限制在 [minmum, maxmum] 内。三个参数都必须只传入无副作用的表达式。 */
#define restrict(in,minmum,maxmum) (in<minmum)?minmum:((in>maxmum)?maxmum:in)


typedef struct LowPassFilter
{
    float Tf;                    /* 时间常数，单位：秒；越大则滤波越平滑、响应越慢 */
    unsigned long timestamp_prev;/* 上一次滤波的时间戳，单位：微秒 */
    float y_prev;                /* 上一次滤波后的输出值 */
}LowPassFilter;
float LowPassFilterRun(LowPassFilter *Filter,float x);
/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
