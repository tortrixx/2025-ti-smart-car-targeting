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


#define GPIO_HFCLKIN_PORT                                                  GPIOA
#define GPIO_HFCLKIN_PIN                                           DL_GPIO_PIN_6
#define GPIO_HFCLKIN_IOMUX                                       (IOMUX_PINCM11)
#define GPIO_HFCLKIN_IOMUX_FUNC                  IOMUX_PINCM11_PF_SYSCTL_HFCLKIN
#define CPUCLK_FREQ                                                     32000000



/* Defines for motor */
#define motor_INST                                                         TIMA0
#define motor_INST_IRQHandler                                   TIMA0_IRQHandler
#define motor_INST_INT_IRQN                                     (TIMA0_INT_IRQn)
#define motor_INST_CLK_FREQ                                             32000000
/* GPIO defines for channel 0 */
#define GPIO_motor_C0_PORT                                                 GPIOA
#define GPIO_motor_C0_PIN                                          DL_GPIO_PIN_0
#define GPIO_motor_C0_IOMUX                                       (IOMUX_PINCM1)
#define GPIO_motor_C0_IOMUX_FUNC                      IOMUX_PINCM1_PF_TIMA0_CCP0
#define GPIO_motor_C0_IDX                                    DL_TIMER_CC_0_INDEX
/* GPIO defines for channel 1 */
#define GPIO_motor_C1_PORT                                                 GPIOA
#define GPIO_motor_C1_PIN                                          DL_GPIO_PIN_1
#define GPIO_motor_C1_IOMUX                                       (IOMUX_PINCM2)
#define GPIO_motor_C1_IOMUX_FUNC                      IOMUX_PINCM2_PF_TIMA0_CCP1
#define GPIO_motor_C1_IDX                                    DL_TIMER_CC_1_INDEX



/* Defines for PIT_FOR_CUSTOM */
#define PIT_FOR_CUSTOM_INST                                             (TIMG12)
#define PIT_FOR_CUSTOM_INST_IRQHandler                         TIMG12_IRQHandler
#define PIT_FOR_CUSTOM_INST_INT_IRQN                           (TIMG12_INT_IRQn)
#define PIT_FOR_CUSTOM_INST_LOAD_VALUE                                  (31999U)



/* Defines for UART_0 */
#define UART_0_INST                                                        UART2
#define UART_0_INST_FREQUENCY                                           32000000
#define UART_0_INST_IRQHandler                                  UART2_IRQHandler
#define UART_0_INST_INT_IRQN                                      UART2_INT_IRQn
#define GPIO_UART_0_RX_PORT                                                GPIOA
#define GPIO_UART_0_TX_PORT                                                GPIOB
#define GPIO_UART_0_RX_PIN                                        DL_GPIO_PIN_24
#define GPIO_UART_0_TX_PIN                                        DL_GPIO_PIN_17
#define GPIO_UART_0_IOMUX_RX                                     (IOMUX_PINCM54)
#define GPIO_UART_0_IOMUX_TX                                     (IOMUX_PINCM43)
#define GPIO_UART_0_IOMUX_RX_FUNC                      IOMUX_PINCM54_PF_UART2_RX
#define GPIO_UART_0_IOMUX_TX_FUNC                      IOMUX_PINCM43_PF_UART2_TX
#define UART_0_BAUD_RATE                                                (115200)
#define UART_0_IBRD_32_MHZ_115200_BAUD                                      (17)
#define UART_0_FBRD_32_MHZ_115200_BAUD                                      (23)




/* Defines for SPI_FOR_IMU */
#define SPI_FOR_IMU_INST                                                   SPI0
#define SPI_FOR_IMU_INST_IRQHandler                             SPI0_IRQHandler
#define SPI_FOR_IMU_INST_INT_IRQN                                 SPI0_INT_IRQn
#define GPIO_SPI_FOR_IMU_PICO_PORT                                        GPIOA
#define GPIO_SPI_FOR_IMU_PICO_PIN                                 DL_GPIO_PIN_9
#define GPIO_SPI_FOR_IMU_IOMUX_PICO                             (IOMUX_PINCM20)
#define GPIO_SPI_FOR_IMU_IOMUX_PICO_FUNC             IOMUX_PINCM20_PF_SPI0_PICO
#define GPIO_SPI_FOR_IMU_POCI_PORT                                        GPIOA
#define GPIO_SPI_FOR_IMU_POCI_PIN                                DL_GPIO_PIN_10
#define GPIO_SPI_FOR_IMU_IOMUX_POCI                             (IOMUX_PINCM21)
#define GPIO_SPI_FOR_IMU_IOMUX_POCI_FUNC             IOMUX_PINCM21_PF_SPI0_POCI
/* GPIO configuration for SPI_FOR_IMU */
#define GPIO_SPI_FOR_IMU_SCLK_PORT                                        GPIOA
#define GPIO_SPI_FOR_IMU_SCLK_PIN                                DL_GPIO_PIN_11
#define GPIO_SPI_FOR_IMU_IOMUX_SCLK                             (IOMUX_PINCM22)
#define GPIO_SPI_FOR_IMU_IOMUX_SCLK_FUNC             IOMUX_PINCM22_PF_SPI0_SCLK
#define GPIO_SPI_FOR_IMU_CS0_PORT                                         GPIOA
#define GPIO_SPI_FOR_IMU_CS0_PIN                                  DL_GPIO_PIN_2
#define GPIO_SPI_FOR_IMU_IOMUX_CS0                               (IOMUX_PINCM7)
#define GPIO_SPI_FOR_IMU_IOMUX_CS0_FUNC                IOMUX_PINCM7_PF_SPI0_CS0



/* Port definition for Pin Group SPI_FOR_IMU_CS */
#define SPI_FOR_IMU_CS_PORT                                              (GPIOA)

/* Defines for PIN_8: GPIOA.8 with pinCMx 19 on package pin 54 */
#define SPI_FOR_IMU_CS_PIN_8_PIN                                 (DL_GPIO_PIN_8)
#define SPI_FOR_IMU_CS_PIN_8_IOMUX                               (IOMUX_PINCM19)
/* Port definition for Pin Group been */
#define been_PORT                                                        (GPIOA)

/* Defines for PIN_10: GPIOA.12 with pinCMx 34 on package pin 5 */
#define been_PIN_10_PIN                                         (DL_GPIO_PIN_12)
#define been_PIN_10_IOMUX                                        (IOMUX_PINCM34)
/* Port definition for Pin Group GPIO_OLED */
#define GPIO_OLED_PORT                                                   (GPIOA)

/* Defines for OLED_SCL: GPIOA.27 with pinCMx 60 on package pin 31 */
#define GPIO_OLED_OLED_SCL_PIN                                  (DL_GPIO_PIN_27)
#define GPIO_OLED_OLED_SCL_IOMUX                                 (IOMUX_PINCM60)
/* Defines for OLED_SDA: GPIOA.26 with pinCMx 59 on package pin 30 */
#define GPIO_OLED_OLED_SDA_PIN                                  (DL_GPIO_PIN_26)
#define GPIO_OLED_OLED_SDA_IOMUX                                 (IOMUX_PINCM59)
/* Port definition for Pin Group GPIO_Encoders */
#define GPIO_Encoders_PORT                                               (GPIOB)

/* Defines for M2_B0_TI1: GPIOB.0 with pinCMx 12 on package pin 47 */
// pins affected by this interrupt request:["M2_B0_TI1","M2_B1_TI2","M1_B3_TI1","M1_B2_TI2"]
#define GPIO_Encoders_INT_IRQN                                  (GPIOB_INT_IRQn)
#define GPIO_Encoders_INT_IIDX                  (DL_INTERRUPT_GROUP1_IIDX_GPIOB)
#define GPIO_Encoders_M2_B0_TI1_IIDX                         (DL_GPIO_IIDX_DIO0)
#define GPIO_Encoders_M2_B0_TI1_PIN                              (DL_GPIO_PIN_0)
#define GPIO_Encoders_M2_B0_TI1_IOMUX                            (IOMUX_PINCM12)
/* Defines for M2_B1_TI2: GPIOB.1 with pinCMx 13 on package pin 48 */
#define GPIO_Encoders_M2_B1_TI2_IIDX                         (DL_GPIO_IIDX_DIO1)
#define GPIO_Encoders_M2_B1_TI2_PIN                              (DL_GPIO_PIN_1)
#define GPIO_Encoders_M2_B1_TI2_IOMUX                            (IOMUX_PINCM13)
/* Defines for M1_B3_TI1: GPIOB.3 with pinCMx 16 on package pin 51 */
#define GPIO_Encoders_M1_B3_TI1_IIDX                         (DL_GPIO_IIDX_DIO3)
#define GPIO_Encoders_M1_B3_TI1_PIN                              (DL_GPIO_PIN_3)
#define GPIO_Encoders_M1_B3_TI1_IOMUX                            (IOMUX_PINCM16)
/* Defines for M1_B2_TI2: GPIOB.2 with pinCMx 15 on package pin 50 */
#define GPIO_Encoders_M1_B2_TI2_IIDX                         (DL_GPIO_IIDX_DIO2)
#define GPIO_Encoders_M1_B2_TI2_PIN                              (DL_GPIO_PIN_2)
#define GPIO_Encoders_M1_B2_TI2_IOMUX                            (IOMUX_PINCM15)
/* Port definition for Pin Group motor_control */
#define motor_control_PORT                                               (GPIOA)

/* Defines for PIN_2: GPIOA.31 with pinCMx 6 on package pin 39 */
#define motor_control_PIN_2_PIN                                 (DL_GPIO_PIN_31)
#define motor_control_PIN_2_IOMUX                                 (IOMUX_PINCM6)
/* Defines for PIN_3: GPIOA.30 with pinCMx 5 on package pin 37 */
#define motor_control_PIN_3_PIN                                 (DL_GPIO_PIN_30)
#define motor_control_PIN_3_IOMUX                                 (IOMUX_PINCM5)
/* Defines for PIN_4: GPIOA.29 with pinCMx 4 on package pin 36 */
#define motor_control_PIN_4_PIN                                 (DL_GPIO_PIN_29)
#define motor_control_PIN_4_IOMUX                                 (IOMUX_PINCM4)
/* Defines for PIN_5: GPIOA.28 with pinCMx 3 on package pin 35 */
#define motor_control_PIN_5_PIN                                 (DL_GPIO_PIN_28)
#define motor_control_PIN_5_IOMUX                                 (IOMUX_PINCM3)
/* Defines for PIN_0: GPIOB.7 with pinCMx 24 on package pin 59 */
#define GPIO_key_PIN_0_PORT                                              (GPIOB)
#define GPIO_key_PIN_0_PIN                                       (DL_GPIO_PIN_7)
#define GPIO_key_PIN_0_IOMUX                                     (IOMUX_PINCM24)
/* Defines for PIN_1: GPIOB.8 with pinCMx 25 on package pin 60 */
#define GPIO_key_PIN_1_PORT                                              (GPIOB)
#define GPIO_key_PIN_1_PIN                                       (DL_GPIO_PIN_8)
#define GPIO_key_PIN_1_IOMUX                                     (IOMUX_PINCM25)
/* Defines for PIN_6: GPIOB.9 with pinCMx 26 on package pin 61 */
#define GPIO_key_PIN_6_PORT                                              (GPIOB)
#define GPIO_key_PIN_6_PIN                                       (DL_GPIO_PIN_9)
#define GPIO_key_PIN_6_IOMUX                                     (IOMUX_PINCM26)
/* Defines for PIN_7: GPIOB.10 with pinCMx 27 on package pin 62 */
#define GPIO_key_PIN_7_PORT                                              (GPIOB)
#define GPIO_key_PIN_7_PIN                                      (DL_GPIO_PIN_10)
#define GPIO_key_PIN_7_IOMUX                                     (IOMUX_PINCM27)
/* Defines for PIN_9: GPIOA.15 with pinCMx 37 on package pin 8 */
#define GPIO_key_PIN_9_PORT                                              (GPIOA)
#define GPIO_key_PIN_9_PIN                                      (DL_GPIO_PIN_15)
#define GPIO_key_PIN_9_IOMUX                                     (IOMUX_PINCM37)

/* clang-format on */

void SYSCFG_DL_init(void);
void SYSCFG_DL_initPower(void);
void SYSCFG_DL_GPIO_init(void);
void SYSCFG_DL_SYSCTL_init(void);
void SYSCFG_DL_motor_init(void);
void SYSCFG_DL_PIT_FOR_CUSTOM_init(void);
void SYSCFG_DL_UART_0_init(void);
void SYSCFG_DL_SPI_FOR_IMU_init(void);


bool SYSCFG_DL_saveConfiguration(void);
bool SYSCFG_DL_restoreConfiguration(void);

#ifdef __cplusplus
}
#endif

#endif /* ti_msp_dl_config_h */
