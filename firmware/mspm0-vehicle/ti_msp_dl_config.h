/*
 * Copyright (c) 2023, Texas Instruments Incorporated - http://www.ti.com
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *  ============ ti_msp_dl_config.h =============
 *  Configured MSPM0 DriverLib module declarations
 *
 *  DO NOT EDIT - This file is generated for the MSPM0G350X
 *  by the SysConfig tool.
 */
#ifndef ti_msp_dl_config_h
#define ti_msp_dl_config_h

#define CONFIG_MSPM0G350X
#define CONFIG_MSPM0G3507

#if defined(__ti_version__) || defined(__TI_COMPILER_VERSION__)
#define SYSCONFIG_WEAK __attribute__((weak))
#elif defined(__IAR_SYSTEMS_ICC__)
#define SYSCONFIG_WEAK __weak
#elif defined(__GNUC__)
#define SYSCONFIG_WEAK __attribute__((weak))
#endif

#include <ti/devices/msp/msp.h>
#include <ti/driverlib/driverlib.h>
#include <ti/driverlib/m0p/dl_core.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  ======== SYSCFG_DL_init ========
 *  Perform all required MSP DL initialization
 *
 *  This function should be called once at a point before any use of
 *  MSP DL.
 */


/* clang-format off */

#define POWER_STARTUP_DELAY                                                (16)


#define CPUCLK_FREQ                                                     32000000



/* Defines for PWM_LEFT */
#define PWM_LEFT_INST                                                      TIMG0
#define PWM_LEFT_INST_IRQHandler                                TIMG0_IRQHandler
#define PWM_LEFT_INST_INT_IRQN                                  (TIMG0_INT_IRQn)
#define PWM_LEFT_INST_CLK_FREQ                                           2000000
/* GPIO defines for channel 0 */
#define GPIO_PWM_LEFT_C0_PORT                                              GPIOA
#define GPIO_PWM_LEFT_C0_PIN                                      DL_GPIO_PIN_12
#define GPIO_PWM_LEFT_C0_IOMUX                                   (IOMUX_PINCM34)
#define GPIO_PWM_LEFT_C0_IOMUX_FUNC                  IOMUX_PINCM34_PF_TIMG0_CCP0
#define GPIO_PWM_LEFT_C0_IDX                                 DL_TIMER_CC_0_INDEX
/* GPIO defines for channel 1 */
#define GPIO_PWM_LEFT_C1_PORT                                              GPIOA
#define GPIO_PWM_LEFT_C1_PIN                                      DL_GPIO_PIN_13
#define GPIO_PWM_LEFT_C1_IOMUX                                   (IOMUX_PINCM35)
#define GPIO_PWM_LEFT_C1_IOMUX_FUNC                  IOMUX_PINCM35_PF_TIMG0_CCP1
#define GPIO_PWM_LEFT_C1_IDX                                 DL_TIMER_CC_1_INDEX

/* Defines for PWM_RIGHT */
#define PWM_RIGHT_INST                                                     TIMG7
#define PWM_RIGHT_INST_IRQHandler                               TIMG7_IRQHandler
#define PWM_RIGHT_INST_INT_IRQN                                 (TIMG7_INT_IRQn)
#define PWM_RIGHT_INST_CLK_FREQ                                          2000000
/* GPIO defines for channel 0 */
#define GPIO_PWM_RIGHT_C0_PORT                                             GPIOA
#define GPIO_PWM_RIGHT_C0_PIN                                     DL_GPIO_PIN_28
#define GPIO_PWM_RIGHT_C0_IOMUX                                   (IOMUX_PINCM3)
#define GPIO_PWM_RIGHT_C0_IOMUX_FUNC                  IOMUX_PINCM3_PF_TIMG7_CCP0
#define GPIO_PWM_RIGHT_C0_IDX                                DL_TIMER_CC_0_INDEX
/* GPIO defines for channel 1 */
#define GPIO_PWM_RIGHT_C1_PORT                                             GPIOA
#define GPIO_PWM_RIGHT_C1_PIN                                     DL_GPIO_PIN_31
#define GPIO_PWM_RIGHT_C1_IOMUX                                   (IOMUX_PINCM6)
#define GPIO_PWM_RIGHT_C1_IOMUX_FUNC                  IOMUX_PINCM6_PF_TIMG7_CCP1
#define GPIO_PWM_RIGHT_C1_IDX                                DL_TIMER_CC_1_INDEX




/* Port definition for Pin Group GPIO_GRP_1 */
#define GPIO_GRP_1_PORT                                                  (GPIOB)

/* Defines for PIN_1: GPIOB.15 with pinCMx 32 on package pin 3 */
#define GPIO_GRP_1_PIN_1_PIN                                    (DL_GPIO_PIN_15)
#define GPIO_GRP_1_PIN_1_IOMUX                                   (IOMUX_PINCM32)
/* Port definition for Pin Group GPIO_GRP_2 */
#define GPIO_GRP_2_PORT                                                  (GPIOB)

/* Defines for PIN_2: GPIOB.14 with pinCMx 31 on package pin 2 */
#define GPIO_GRP_2_PIN_2_PIN                                    (DL_GPIO_PIN_14)
#define GPIO_GRP_2_PIN_2_IOMUX                                   (IOMUX_PINCM31)
/* Port definition for Pin Group GPIO_GRP_3 */
#define GPIO_GRP_3_PORT                                                  (GPIOB)

/* Defines for PIN_3: GPIOB.13 with pinCMx 30 on package pin 1 */
#define GPIO_GRP_3_PIN_3_PIN                                    (DL_GPIO_PIN_13)
#define GPIO_GRP_3_PIN_3_IOMUX                                   (IOMUX_PINCM30)
/* Defines for LINE_X1: GPIOA.15 with pinCMx 37 on package pin 8 */
#define GPIO_LINE_LINE_X1_PORT                                           (GPIOA)
#define GPIO_LINE_LINE_X1_PIN                                   (DL_GPIO_PIN_15)
#define GPIO_LINE_LINE_X1_IOMUX                                  (IOMUX_PINCM37)
/* Defines for LINE_X2: GPIOA.17 with pinCMx 39 on package pin 10 */
#define GPIO_LINE_LINE_X2_PORT                                           (GPIOA)
#define GPIO_LINE_LINE_X2_PIN                                   (DL_GPIO_PIN_17)
#define GPIO_LINE_LINE_X2_IOMUX                                  (IOMUX_PINCM39)
/* Defines for LINE_X3: GPIOA.22 with pinCMx 47 on package pin 18 */
#define GPIO_LINE_LINE_X3_PORT                                           (GPIOA)
#define GPIO_LINE_LINE_X3_PIN                                   (DL_GPIO_PIN_22)
#define GPIO_LINE_LINE_X3_IOMUX                                  (IOMUX_PINCM47)
/* Defines for LINE_X4: GPIOA.24 with pinCMx 54 on package pin 25 */
#define GPIO_LINE_LINE_X4_PORT                                           (GPIOA)
#define GPIO_LINE_LINE_X4_PIN                                   (DL_GPIO_PIN_24)
#define GPIO_LINE_LINE_X4_IOMUX                                  (IOMUX_PINCM54)
/* Defines for LINE_X5: GPIOA.25 with pinCMx 55 on package pin 26 */
#define GPIO_LINE_LINE_X5_PORT                                           (GPIOA)
#define GPIO_LINE_LINE_X5_PIN                                   (DL_GPIO_PIN_25)
#define GPIO_LINE_LINE_X5_IOMUX                                  (IOMUX_PINCM55)
/* Defines for LINE_X6: GPIOA.26 with pinCMx 59 on package pin 30 */
#define GPIO_LINE_LINE_X6_PORT                                           (GPIOA)
#define GPIO_LINE_LINE_X6_PIN                                   (DL_GPIO_PIN_26)
#define GPIO_LINE_LINE_X6_IOMUX                                  (IOMUX_PINCM59)
/* Defines for LINE_X7: GPIOA.27 with pinCMx 60 on package pin 31 */
#define GPIO_LINE_LINE_X7_PORT                                           (GPIOA)
#define GPIO_LINE_LINE_X7_PIN                                   (DL_GPIO_PIN_27)
#define GPIO_LINE_LINE_X7_IOMUX                                  (IOMUX_PINCM60)
/* Defines for LINE_X8: GPIOB.20 with pinCMx 48 on package pin 19 */
#define GPIO_LINE_LINE_X8_PORT                                           (GPIOB)
#define GPIO_LINE_LINE_X8_PIN                                   (DL_GPIO_PIN_20)
#define GPIO_LINE_LINE_X8_IOMUX                                  (IOMUX_PINCM48)


/* clang-format on */

void SYSCFG_DL_init(void);
void SYSCFG_DL_initPower(void);
void SYSCFG_DL_GPIO_init(void);
void SYSCFG_DL_SYSCTL_init(void);
void SYSCFG_DL_PWM_LEFT_init(void);
void SYSCFG_DL_PWM_RIGHT_init(void);


bool SYSCFG_DL_saveConfiguration(void);
bool SYSCFG_DL_restoreConfiguration(void);

#ifdef __cplusplus
}
#endif

#endif /* ti_msp_dl_config_h */
