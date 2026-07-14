/*
 * Copyright (c) 2020, Texas Instruments Incorporated
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

#include "ti_msp_dl_config.h"
#include "board.h"
#include "drv_oled.h"


volatile bool gCheckADC;
volatile uint16_t gAdcResult;

uint16_t da=0;

int ADV[150]={0};      //整型数组，储存从左到右128个像素的灰度值//
uint8_t Flag_Target;
#define TSL_CLK(i)  i?DL_GPIO_setPins(GPIOA,DL_GPIO_PIN_26):DL_GPIO_clearPins(GPIOA,DL_GPIO_PIN_26)
#define TSL_SI(i)   i?DL_GPIO_setPins(GPIOA,DL_GPIO_PIN_27):DL_GPIO_clearPins(GPIOA,DL_GPIO_PIN_27)







uint16_t Get_Adc()
{
    DL_ADC12_startConversion(ADC12_0_INST);

    while (false == gCheckADC);

    gAdcResult = DL_ADC12_getMemResult(ADC12_0_INST, DL_ADC12_MEM_IDX_0);


    gCheckADC = false;
    DL_ADC12_enableConversions(ADC12_0_INST);
    return gAdcResult;
}

uint8_t pixel_binary[128] = {0}; //灰度
int threshold =0;
uint8_t center=0;
uint8_t black_cnt=0,black_cntout=0;
uint8_t FFF=0;
void calcu_center()
{
    int max= 0;
    int min =10000;
    int sumweight =0;
    int num = 0;
    for (uint8_t i =15; i<128; i++) {
        max = (ADV[i]<max)?max:ADV[i];
        min = (ADV[i]>min)?min:ADV[i];
    }
    

    threshold = (max+min)/2;
    if((max-min)>200)
    {
        for (uint8_t i =15; i<128; i++) {
            if(ADV[i]<threshold)
            {
                pixel_binary[i] = 1;
                sumweight += i;
                num++;
            }
            else {
            pixel_binary[i]=0;
            }
        }
        center = sumweight/num;
				
				for(uint8_t i =15; i<128; i++)
				{
				
				if(pixel_binary[i])
				{	
				black_cnt++;	
				}	
				else
				{
				if(black_cnt>black_cntout)
				{black_cntout=black_cnt;}
				black_cnt=0;
				if(pixel_binary[i-1])
				FFF++;
				}
				
				}

				//black_cntout+=128;
				black_cntout|=128;
				DL_UART_transmitDataBlocking(UART_0_INST, black_cntout);		
				DL_UART_transmitDataBlocking(UART_0_INST, center);
				black_cntout=0;
        //DL_UART_transmitData(UART_0_INST, center);
    }



}





 void RD_TSL(void)
{
 uint8_t i=0,tslp=0;
  TSL_CLK(0);
  TSL_SI(0); 
  delay_us(5);
	
      
   

	TSL_SI(1);
delay_us(30);
	
	 TSL_CLK(1);
  delay_us(5);
	TSL_SI(0);

  delay_us(5);
  for(i=0;i<128;i++)
  { 
    TSL_CLK(0); 
    delay_us(40);
    ADV[tslp]=(Get_Adc());
    ++tslp;
    TSL_CLK(1);
     delay_us(10);
  } 
  calcu_center();
}


int main(void)
{
    SYSCFG_DL_init();
		oled_init();
		NVIC_EnableIRQ(TIMER_0_INST_INT_IRQN);
    DL_TimerG_startCounter(TIMER_0_INST);
    NVIC_EnableIRQ(ADC12_0_INST_INT_IRQN);
    gCheckADC = false;
    while (1) {
    }
}

void ADC12_0_INST_IRQHandler(void)
{
    switch (DL_ADC12_getPendingInterrupt(ADC12_0_INST)) {
        case DL_ADC12_IIDX_MEM0_RESULT_LOADED:
            gCheckADC = true;
            break;
        default:
            break;
    }
}

uint8_t x=0;
uint16_t ccc=0;
void TIMER_0_INST_IRQHandler(void)
{
    RD_TSL();
	
		

		if(ccc<200)
	{
		ccc++;
	  OLED_WrCmd(0xb0);
    OLED_WrCmd(0x01);
    OLED_WrCmd(0x10);
    for(x=0;x<X_WIDTH;x++)
	{
		if(pixel_binary[x])
    OLED_WrDat(0);
		else
	  OLED_WrDat(0XFF);
	}
	
		OLED_WrCmd(0xb0+1);
    OLED_WrCmd(0x01);
    OLED_WrCmd(0x10);
    for(x=0;x<X_WIDTH;x++)
	{
		if(x!=center)
    OLED_WrDat(0);
		else
	  OLED_WrDat(0XFF);
	}
	
		}
	

DL_TimerG_startCounter(TIMER_0_INST);
}