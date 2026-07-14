/*
 * Copyright (c) 2023, Texas Instruments Incorporated
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
 *  ============ ti_msp_dl_config.c =============
 *  Configured MSPM0 DriverLib module definitions
 *
 *  DO NOT EDIT - This file is generated for the MSPM0G350X
 *  by the SysConfig tool.
 */

#include "ti_msp_dl_config.h"

DL_TimerA_backupConfig gmotorBackup;
DL_SPI_backupConfig gSPI_FOR_IMUBackup;

/*
 *  ======== SYSCFG_DL_init ========
 *  Perform any initialization needed before using any board APIs
 */
SYSCONFIG_WEAK void SYSCFG_DL_init(void)
{
    SYSCFG_DL_initPower();
    SYSCFG_DL_GPIO_init();
    /* Module-Specific Initializations*/
    SYSCFG_DL_SYSCTL_init();
    SYSCFG_DL_motor_init();
    SYSCFG_DL_PIT_FOR_CUSTOM_init();
    SYSCFG_DL_UART_0_init();
    SYSCFG_DL_SPI_FOR_IMU_init();
    /* Ensure backup structures have no valid state */
	gmotorBackup.backupRdy 	= false;


    
	gSPI_FOR_IMUBackup.backupRdy 	= false;

}
/*
 * User should take care to save and restore register configuration in application.
 * See Retention Configuration section for more details.
 */
SYSCONFIG_WEAK bool SYSCFG_DL_saveConfiguration(void)
{
    bool retStatus = true;

	retStatus &= DL_TimerA_saveConfiguration(motor_INST, &gmotorBackup);
	retStatus &= DL_SPI_saveConfiguration(SPI_FOR_IMU_INST, &gSPI_FOR_IMUBackup);

    return retStatus;
}


SYSCONFIG_WEAK bool SYSCFG_DL_restoreConfiguration(void)
{
    bool retStatus = true;

	retStatus &= DL_TimerA_restoreConfiguration(motor_INST, &gmotorBackup, false);
	retStatus &= DL_SPI_restoreConfiguration(SPI_FOR_IMU_INST, &gSPI_FOR_IMUBackup);

    return retStatus;
}

SYSCONFIG_WEAK void SYSCFG_DL_initPower(void)
{
    DL_GPIO_reset(GPIOA);
    DL_GPIO_reset(GPIOB);
    DL_TimerA_reset(motor_INST);
    DL_TimerG_reset(PIT_FOR_CUSTOM_INST);
    DL_UART_Main_reset(UART_0_INST);
    DL_SPI_reset(SPI_FOR_IMU_INST);

    DL_GPIO_enablePower(GPIOA);
    DL_GPIO_enablePower(GPIOB);
    DL_TimerA_enablePower(motor_INST);
    DL_TimerG_enablePower(PIT_FOR_CUSTOM_INST);
    DL_UART_Main_enablePower(UART_0_INST);
    DL_SPI_enablePower(SPI_FOR_IMU_INST);
    delay_cycles(POWER_STARTUP_DELAY);
}

SYSCONFIG_WEAK void SYSCFG_DL_GPIO_init(void)
{

    DL_GPIO_initPeripheralInputFunction(GPIO_HFCLKIN_IOMUX, GPIO_HFCLKIN_IOMUX_FUNC);

    DL_GPIO_initPeripheralOutputFunction(GPIO_motor_C0_IOMUX,GPIO_motor_C0_IOMUX_FUNC);
    DL_GPIO_enableOutput(GPIO_motor_C0_PORT, GPIO_motor_C0_PIN);
    DL_GPIO_initPeripheralOutputFunction(GPIO_motor_C1_IOMUX,GPIO_motor_C1_IOMUX_FUNC);
    DL_GPIO_enableOutput(GPIO_motor_C1_PORT, GPIO_motor_C1_PIN);

    DL_GPIO_initPeripheralOutputFunction(
        GPIO_UART_0_IOMUX_TX, GPIO_UART_0_IOMUX_TX_FUNC);
    DL_GPIO_initPeripheralInputFunction(
        GPIO_UART_0_IOMUX_RX, GPIO_UART_0_IOMUX_RX_FUNC);

    DL_GPIO_initPeripheralOutputFunction(
        GPIO_SPI_FOR_IMU_IOMUX_SCLK, GPIO_SPI_FOR_IMU_IOMUX_SCLK_FUNC);
    DL_GPIO_initPeripheralOutputFunction(
        GPIO_SPI_FOR_IMU_IOMUX_PICO, GPIO_SPI_FOR_IMU_IOMUX_PICO_FUNC);
    DL_GPIO_initPeripheralInputFunction(
        GPIO_SPI_FOR_IMU_IOMUX_POCI, GPIO_SPI_FOR_IMU_IOMUX_POCI_FUNC);
    DL_GPIO_initPeripheralOutputFunction(
        GPIO_SPI_FOR_IMU_IOMUX_CS0, GPIO_SPI_FOR_IMU_IOMUX_CS0_FUNC);

    DL_GPIO_initDigitalOutput(SPI_FOR_IMU_CS_PIN_8_IOMUX);

    DL_GPIO_initDigitalOutput(been_PIN_10_IOMUX);

    DL_GPIO_initDigitalOutput(GPIO_OLED_OLED_SCL_IOMUX);

    DL_GPIO_initDigitalOutput(GPIO_OLED_OLED_SDA_IOMUX);

    DL_GPIO_initDigitalInputFeatures(GPIO_Encoders_M2_B0_TI1_IOMUX,
		 DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_NONE,
		 DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);

    DL_GPIO_initDigitalInputFeatures(GPIO_Encoders_M2_B1_TI2_IOMUX,
		 DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_NONE,
		 DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);

    DL_GPIO_initDigitalInputFeatures(GPIO_Encoders_M1_B3_TI1_IOMUX,
		 DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_NONE,
		 DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);

    DL_GPIO_initDigitalInputFeatures(GPIO_Encoders_M1_B2_TI2_IOMUX,
		 DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_NONE,
		 DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);

    DL_GPIO_initDigitalOutput(motor_control_PIN_2_IOMUX);

    DL_GPIO_initDigitalOutput(motor_control_PIN_3_IOMUX);

    DL_GPIO_initDigitalOutput(motor_control_PIN_4_IOMUX);

    DL_GPIO_initDigitalOutput(motor_control_PIN_5_IOMUX);

    DL_GPIO_initDigitalInputFeatures(GPIO_key_PIN_0_IOMUX,
		 DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_PULL_UP,
		 DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);

    DL_GPIO_initDigitalInputFeatures(GPIO_key_PIN_1_IOMUX,
		 DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_PULL_UP,
		 DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);

    DL_GPIO_initDigitalInputFeatures(GPIO_key_PIN_6_IOMUX,
		 DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_PULL_UP,
		 DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);

    DL_GPIO_initDigitalInputFeatures(GPIO_key_PIN_7_IOMUX,
		 DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_PULL_UP,
		 DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);

    DL_GPIO_initDigitalInputFeatures(GPIO_key_PIN_9_IOMUX,
		 DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_PULL_UP,
		 DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);

    DL_GPIO_clearPins(GPIOA, SPI_FOR_IMU_CS_PIN_8_PIN |
		GPIO_OLED_OLED_SCL_PIN |
		GPIO_OLED_OLED_SDA_PIN |
		motor_control_PIN_2_PIN |
		motor_control_PIN_3_PIN |
		motor_control_PIN_4_PIN |
		motor_control_PIN_5_PIN);
    DL_GPIO_setPins(GPIOA, been_PIN_10_PIN);
    DL_GPIO_enableOutput(GPIOA, SPI_FOR_IMU_CS_PIN_8_PIN |
		been_PIN_10_PIN |
		GPIO_OLED_OLED_SCL_PIN |
		GPIO_OLED_OLED_SDA_PIN |
		motor_control_PIN_2_PIN |
		motor_control_PIN_3_PIN |
		motor_control_PIN_4_PIN |
		motor_control_PIN_5_PIN);
    DL_GPIO_setLowerPinsPolarity(GPIOB, DL_GPIO_PIN_0_EDGE_RISE_FALL |
		DL_GPIO_PIN_1_EDGE_RISE_FALL |
		DL_GPIO_PIN_3_EDGE_RISE_FALL |
		DL_GPIO_PIN_2_EDGE_RISE_FALL);
    DL_GPIO_clearInterruptStatus(GPIOB, GPIO_Encoders_M2_B0_TI1_PIN |
		GPIO_Encoders_M2_B1_TI2_PIN |
		GPIO_Encoders_M1_B3_TI1_PIN |
		GPIO_Encoders_M1_B2_TI2_PIN);
    DL_GPIO_enableInterrupt(GPIOB, GPIO_Encoders_M2_B0_TI1_PIN |
		GPIO_Encoders_M2_B1_TI2_PIN |
		GPIO_Encoders_M1_B3_TI1_PIN |
		GPIO_Encoders_M1_B2_TI2_PIN);

}



SYSCONFIG_WEAK void SYSCFG_DL_SYSCTL_init(void)
{

	//Low Power Mode is configured to be SLEEP0
    DL_SYSCTL_setBORThreshold(DL_SYSCTL_BOR_THRESHOLD_LEVEL_0);

    
	DL_SYSCTL_setSYSOSCFreq(DL_SYSCTL_SYSOSC_FREQ_BASE);
    DL_SYSCTL_setHFCLKSourceHFCLKIN();

}


/*
 * Timer clock configuration to be sourced by  / 1 (32000000 Hz)
 * timerClkFreq = (timerClkSrc / (timerClkDivRatio * (timerClkPrescale + 1)))
 *   32000000 Hz = 32000000 Hz / (1 * (0 + 1))
 */
static const DL_TimerA_ClockConfig gmotorClockConfig = {
    .clockSel = DL_TIMER_CLOCK_BUSCLK,
    .divideRatio = DL_TIMER_CLOCK_DIVIDE_1,
    .prescale = 0U
};

static const DL_TimerA_PWMConfig gmotorConfig = {
    .pwmMode = DL_TIMER_PWM_MODE_EDGE_ALIGN_UP,
    .period = 10000,
    .isTimerWithFourCC = true,
    .startTimer = DL_TIMER_START,
};

SYSCONFIG_WEAK void SYSCFG_DL_motor_init(void) {

    DL_TimerA_setClockConfig(
        motor_INST, (DL_TimerA_ClockConfig *) &gmotorClockConfig);

    DL_TimerA_initPWMMode(
        motor_INST, (DL_TimerA_PWMConfig *) &gmotorConfig);

    // Set Counter control to the smallest CC index being used
    DL_TimerA_setCounterControl(motor_INST,DL_TIMER_CZC_CCCTL0_ZCOND,DL_TIMER_CAC_CCCTL0_ACOND,DL_TIMER_CLC_CCCTL0_LCOND);

    DL_TimerA_setCaptureCompareOutCtl(motor_INST, DL_TIMER_CC_OCTL_INIT_VAL_LOW,
		DL_TIMER_CC_OCTL_INV_OUT_DISABLED, DL_TIMER_CC_OCTL_SRC_FUNCVAL,
		DL_TIMERA_CAPTURE_COMPARE_0_INDEX);

    DL_TimerA_setCaptCompUpdateMethod(motor_INST, DL_TIMER_CC_UPDATE_METHOD_IMMEDIATE, DL_TIMERA_CAPTURE_COMPARE_0_INDEX);
    DL_TimerA_setCaptureCompareValue(motor_INST, 2999, DL_TIMER_CC_0_INDEX);

    DL_TimerA_setCaptureCompareOutCtl(motor_INST, DL_TIMER_CC_OCTL_INIT_VAL_LOW,
		DL_TIMER_CC_OCTL_INV_OUT_DISABLED, DL_TIMER_CC_OCTL_SRC_FUNCVAL,
		DL_TIMERA_CAPTURE_COMPARE_1_INDEX);

    DL_TimerA_setCaptCompUpdateMethod(motor_INST, DL_TIMER_CC_UPDATE_METHOD_IMMEDIATE, DL_TIMERA_CAPTURE_COMPARE_1_INDEX);
    DL_TimerA_setCaptureCompareValue(motor_INST, 2999, DL_TIMER_CC_1_INDEX);

    DL_TimerA_enableClock(motor_INST);


    
    DL_TimerA_setCCPDirection(motor_INST , DL_TIMER_CC0_OUTPUT | DL_TIMER_CC1_OUTPUT );


}



/*
 * Timer clock configuration to be sourced by BUSCLK /  (32000000 Hz)
 * timerClkFreq = (timerClkSrc / (timerClkDivRatio * (timerClkPrescale + 1)))
 *   32000000 Hz = 32000000 Hz / (1 * (0 + 1))
 */
static const DL_TimerG_ClockConfig gPIT_FOR_CUSTOMClockConfig = {
    .clockSel    = DL_TIMER_CLOCK_BUSCLK,
    .divideRatio = DL_TIMER_CLOCK_DIVIDE_1,
    .prescale    = 0U,
};

/*
 * Timer load value (where the counter starts from) is calculated as (timerPeriod * timerClockFreq) - 1
 * PIT_FOR_CUSTOM_INST_LOAD_VALUE = (1ms * 32000000 Hz) - 1
 */
static const DL_TimerG_TimerConfig gPIT_FOR_CUSTOMTimerConfig = {
    .period     = PIT_FOR_CUSTOM_INST_LOAD_VALUE,
    .timerMode  = DL_TIMER_TIMER_MODE_PERIODIC_UP,
    .startTimer = DL_TIMER_STOP,
};

SYSCONFIG_WEAK void SYSCFG_DL_PIT_FOR_CUSTOM_init(void) {

    DL_TimerG_setClockConfig(PIT_FOR_CUSTOM_INST,
        (DL_TimerG_ClockConfig *) &gPIT_FOR_CUSTOMClockConfig);

    DL_TimerG_initTimerMode(PIT_FOR_CUSTOM_INST,
        (DL_TimerG_TimerConfig *) &gPIT_FOR_CUSTOMTimerConfig);
    DL_TimerG_enableInterrupt(PIT_FOR_CUSTOM_INST , DL_TIMERG_INTERRUPT_LOAD_EVENT);
	NVIC_SetPriority(PIT_FOR_CUSTOM_INST_INT_IRQN, 3);
    DL_TimerG_enableClock(PIT_FOR_CUSTOM_INST);





}



static const DL_UART_Main_ClockConfig gUART_0ClockConfig = {
    .clockSel    = DL_UART_MAIN_CLOCK_BUSCLK,
    .divideRatio = DL_UART_MAIN_CLOCK_DIVIDE_RATIO_1
};

static const DL_UART_Main_Config gUART_0Config = {
    .mode        = DL_UART_MAIN_MODE_NORMAL,
    .direction   = DL_UART_MAIN_DIRECTION_TX_RX,
    .flowControl = DL_UART_MAIN_FLOW_CONTROL_NONE,
    .parity      = DL_UART_MAIN_PARITY_NONE,
    .wordLength  = DL_UART_MAIN_WORD_LENGTH_8_BITS,
    .stopBits    = DL_UART_MAIN_STOP_BITS_ONE
};

SYSCONFIG_WEAK void SYSCFG_DL_UART_0_init(void)
{
    DL_UART_Main_setClockConfig(UART_0_INST, (DL_UART_Main_ClockConfig *) &gUART_0ClockConfig);

    DL_UART_Main_init(UART_0_INST, (DL_UART_Main_Config *) &gUART_0Config);
    /*
     * Configure baud rate by setting oversampling and baud rate divisors.
     *  Target baud rate: 115200
     *  Actual baud rate: 115211.52
     */
    DL_UART_Main_setOversampling(UART_0_INST, DL_UART_OVERSAMPLING_RATE_16X);
    DL_UART_Main_setBaudRateDivisor(UART_0_INST, UART_0_IBRD_32_MHZ_115200_BAUD, UART_0_FBRD_32_MHZ_115200_BAUD);


    /* Configure Interrupts */
    DL_UART_Main_enableInterrupt(UART_0_INST,
                                 DL_UART_MAIN_INTERRUPT_RX);


    DL_UART_Main_enable(UART_0_INST);
}

static const DL_SPI_Config gSPI_FOR_IMU_config = {
    .mode        = DL_SPI_MODE_CONTROLLER,
    .frameFormat = DL_SPI_FRAME_FORMAT_MOTO4_POL0_PHA0,
    .parity      = DL_SPI_PARITY_NONE,
    .dataSize    = DL_SPI_DATA_SIZE_8,
    .bitOrder    = DL_SPI_BIT_ORDER_MSB_FIRST,
    .chipSelectPin = DL_SPI_CHIP_SELECT_0,
};

static const DL_SPI_ClockConfig gSPI_FOR_IMU_clockConfig = {
    .clockSel    = DL_SPI_CLOCK_BUSCLK,
    .divideRatio = DL_SPI_CLOCK_DIVIDE_RATIO_1
};

SYSCONFIG_WEAK void SYSCFG_DL_SPI_FOR_IMU_init(void) {
    DL_SPI_setClockConfig(SPI_FOR_IMU_INST, (DL_SPI_ClockConfig *) &gSPI_FOR_IMU_clockConfig);

    DL_SPI_init(SPI_FOR_IMU_INST, (DL_SPI_Config *) &gSPI_FOR_IMU_config);

    /* Configure Controller mode */
    /*
     * Set the bit rate clock divider to generate the serial output clock
     *     outputBitRate = (spiInputClock) / ((1 + SCR) * 2)
     *     8000000 = (32000000)/((1 + 1) * 2)
     */
    DL_SPI_setBitRateSerialClockDivider(SPI_FOR_IMU_INST, 1);
    /* Set RX and TX FIFO threshold levels */
    DL_SPI_setFIFOThreshold(SPI_FOR_IMU_INST, DL_SPI_RX_FIFO_LEVEL_1_2_FULL, DL_SPI_TX_FIFO_LEVEL_1_2_EMPTY);

    /* Enable module */
    DL_SPI_enable(SPI_FOR_IMU_INST);
}

