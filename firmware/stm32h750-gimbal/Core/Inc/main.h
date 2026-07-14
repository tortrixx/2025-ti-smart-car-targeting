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
typedef struct pid_handler{
	float P;
	float I;
  float I2;
	float D;
	float integral;
  float integral2;
	float previous;
  float de;
	float out_limit[2];
  float inte_limit[2];
  float OUT;
  float last_previous;
  float err_limit;
  float acc_Feed;
  uint8_t d_state;
  uint32_t last_time;
	
}pid_handler;

#define restrict(in,minmum,maxmum) (in<minmum)?minmum:((in>maxmum)?maxmum:in)


typedef struct LowPassFilter
{
    float Tf; //!< 低通滤波时间常数
    unsigned long timestamp_prev;  //!< 最后执行时间戳
    float y_prev; //!< 上一个循环中的过滤后的值
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
