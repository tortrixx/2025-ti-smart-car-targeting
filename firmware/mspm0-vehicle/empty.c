/*
 * Copyright (c) 2021, Texas Instruments Incorporated
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

#include "device_imu660ra.h"
#include "poseCalculate.h"
#include "Navigation.h"

#include "drv_oled.h"
#include "encoder.h"
#include "motor.h" 

void uart0_init(void)
{
	NVIC_ClearPendingIRQ(UART_0_INST_INT_IRQN);
	NVIC_EnableIRQ(UART_0_INST_INT_IRQN);
}

uint8_t gEchoData,NOW_black,NOW_black_cnt;
uint32_t cntre=0;


union Data {
int ecogo;
char ecogou8[4];
} data;



uint8_t xuanquan=0,zhuanjiao=0,yvxuanquan=0,Tzhuanjiao;//转圈找这里！！！！！！！！！！！！！！！！!
int main(void)
{
    SYSCFG_DL_init();
		NVIC_ClearPendingIRQ(UART_0_INST_INT_IRQN);
    NVIC_EnableIRQ(UART_0_INST_INT_IRQN);
		oled_init();//oled显示屏初始化
//	
		while(0){}//bmi270初始化
			
		 //Sensor_GyroOffsetCalc();//零飘校准
			
		//Sensor_Gyro_Give_Directly();//零飘输入
		
		
		NVIC_EnableIRQ(PIT_FOR_CUSTOM_INST_INT_IRQN);
    DL_TimerG_startCounter(PIT_FOR_CUSTOM_INST); //定时器中断启用
			
		EXTI_Init();//编码器中断启用
		motor_init();//PWM初始化电机刹车
//		motor_a(2000);
//		motor_b(-2000);
		
			display_6_8_string(0,4,"7");
			display_6_8_string(20,4,"8");
			display_6_8_string(40,4,"9");
			display_6_8_string(60,4,"10");
			display_6_8_string(0,5,"1");
			display_6_8_string(20,5,"2");
			display_6_8_string(40,5,"3");
			display_6_8_string(60,5,"4");
			int cnt1=0,cnt2=0,cnt3=0,cnt4=0;
    while (1) {
		cnt1=0,cnt2=0,cnt3=0,cnt4=0;
		display_6_8_string(0,0,"QUANshu");
		display_6_8_number(50,0,xuanquan);
		display_6_8_string(0,1,"JIAOshu");
		display_6_8_number(50,1,zhuanjiao);
		display_6_8_string(0,2,"YUQUANshu");
		display_6_8_number(70,2,yvxuanquan);
		display_6_8_number(0,6,encoder1_num);display_6_8_number(64,6,NOW_black);
	if(!DL_GPIO_readPins(GPIOB,DL_GPIO_PIN_7))
	{
	while(!DL_GPIO_readPins(GPIOB,DL_GPIO_PIN_7))
	{
	cnt1++;
	delay_ms(5);
	}
	if(cnt1>20)
	{
		if(yvxuanquan<10)
		yvxuanquan+=1;
		else
		yvxuanquan=1;
	}
	}
		
	else if(!DL_GPIO_readPins(GPIOB,DL_GPIO_PIN_8))
	{
	while(!DL_GPIO_readPins(GPIOB,DL_GPIO_PIN_8))
	{
	cnt2++;
	delay_ms(5);
	}
	if(cnt2>20)
	{
		xuanquan=yvxuanquan;
	}
	}
	
	else if(!DL_GPIO_readPins(GPIOB,DL_GPIO_PIN_9))
	{
	while(!DL_GPIO_readPins(GPIOB,DL_GPIO_PIN_9))
	{
	cnt3++;
	delay_ms(5);
	}
	if(cnt3>20)
	{
		xuanquan=0;
	}
	}
	
	else if(!DL_GPIO_readPins(GPIOB,DL_GPIO_PIN_10))
	{
	while(!DL_GPIO_readPins(GPIOB,DL_GPIO_PIN_10))
	{
	cnt4++;
	delay_ms(5);
	}
	if(cnt4>20)
	{
		if(yvxuanquan>1)
		yvxuanquan-=1;
		else
		yvxuanquan=10;
	}
	}
	
			delay_ms(100);
		//printf("%f,%f,%f,%f,%d\r\n",v_A,v_B,Tj_A,Tj_B,encoder1_num);
 
		}
		
 }
 
uint64_t timm,settima,settimb,settim_flaga=1,settim_flagb=1;
 uint8_t flagz=0,zhuangtai=0;
 int eeco1=0,eeco2=0,eecof=0;
//Timer interrupt function(1ms)
void PIT_FOR_CUSTOM_INST_IRQHandler(void)
{
	static int count = 0;
 
    switch( DL_TimerG_getPendingInterrupt(PIT_FOR_CUSTOM_INST) )
    {
        case DL_TIMER_IIDX_LOAD:
//			ticks++;
		    //IMU pose calculate task(every 5ms)
			count++;
				timm++;
			if(count%10==0)
			{
			if(xuanquan)
			{
				if(Tzhuanjiao==0)
				{
//					Tj_A=20;
//					Tj_B=20;
					DL_GPIO_clearPins(GPIOA,DL_GPIO_PIN_12);
				}
				else
					DL_GPIO_setPins(GPIOA,DL_GPIO_PIN_12);
				
				
				Tzhuanjiao=xuanquan*4;
				
				if(DL_GPIO_readPins(GPIOA,DL_GPIO_PIN_15)&&flagz==0)
				{
					flagz=1;
					Tj_A=0;Tj_B=0;Tl_B=0;Tl_A=0;
					eeco1=encoder1_num;
					eeco2=encoder2_num;
				}
				if(flagz==1){
				Tj_A=0;
				Tj_B=20;
				if(zhuanjiao==Tzhuanjiao)
					{
					xuanquan=0;
					zhuanjiao=0;
					DL_GPIO_clearPins(GPIOA,DL_GPIO_PIN_12);
						
					}	
				if(eeco2-encoder2_num>1300)
				{
					eeco2=encoder2_num;eeco1=encoder1_num;
					flagz=0;
					zhuanjiao++;
					if(zhuanjiao%4==1)
					{
//					Tj_A=0;
//					Tj_B=0;
					settim_flaga=0;
					settim_flagb=0;
					}
					else
					{
					settim_flaga=2;
					settim_flagb=2;						
					Tj_A=30;
					Tj_B=30;
					}

				}
				}
					
			}
			else
			{
				DL_GPIO_setPins(GPIOA,DL_GPIO_PIN_12);
			Tj_A=0;Tj_B=0;Tl_B=0;Tl_A=0;
			}
					data.ecogo=-encoder1_num;
					DL_UART_transmitDataBlocking(UART_0_INST, 0xCA);
					DL_UART_transmitDataBlocking(UART_0_INST, 0xAC);
					DL_UART_transmitDataBlocking(UART_0_INST, data.ecogou8[0]);
					DL_UART_transmitDataBlocking(UART_0_INST, data.ecogou8[1]);
					DL_UART_transmitDataBlocking(UART_0_INST, data.ecogou8[2]);
					DL_UART_transmitDataBlocking(UART_0_INST, data.ecogou8[3]);
					DL_UART_transmitDataBlocking(UART_0_INST, flagz+1);
					DL_UART_transmitDataBlocking(UART_0_INST, zhuanjiao);		
					DL_UART_transmitDataBlocking(UART_0_INST, 0x12);
					DL_UART_transmitDataBlocking(UART_0_INST, 0x23);	


//					data.ecogo=-encoder1_num;
//					zhuangtai=((zhuanjiao%4+1)<<4)+flagz;
//					DL_UART_transmitDataBlocking(UART_0_INST, 0xCA);
//					DL_UART_transmitDataBlocking(UART_0_INST, 0xAC);
//					DL_UART_transmitDataBlocking(UART_0_INST, data.ecogou8[0]);
//					DL_UART_transmitDataBlocking(UART_0_INST, data.ecogou8[1]);
//					DL_UART_transmitDataBlocking(UART_0_INST, data.ecogou8[2]);
//					DL_UART_transmitDataBlocking(UART_0_INST, data.ecogou8[3]);
//					DL_UART_transmitDataBlocking(UART_0_INST, zhuangtai);		
//					DL_UART_transmitDataBlocking(UART_0_INST, 0x12);
//					DL_UART_transmitDataBlocking(UART_0_INST, 0x23);

			
			motor_handle();
			}
			if(count%20==0)
			{
				

				
				if(xuanquan)
			{
				if(flagz==0&&NOW_black_cnt<=35)
				findli(61,NOW_black);  //61
				if(flagz==0)
				{
				if(settim_flaga==0){settim_flaga=1;settima=timm;}if(settim_flaga==1){if(jiasuA(10,25,settima,settima+700,timm)==0){settim_flaga=2;}}	
				if(settim_flagb==0){settim_flagb=1;settimb=timm;}if(settim_flagb==1){if(jiasuB(10,25,settimb,settimb+700,timm)==0){settim_flagb=2;}}
				
				if(eeco2-encoder2_num>5500)
				{
				if(settim_flaga==2){settim_flaga=3;settima=timm;}if(settim_flaga==3){if(jiasuA(35,-25,settima,settima+700,timm)==0){settim_flaga=4;}}	
				if(settim_flagb==2){settim_flagb=3;settimb=timm;}if(settim_flagb==3){if(jiasuB(35,-25,settimb,settimb+700,timm)==0){settim_flagb=4;}}
				}
				
//				if(settim_flaga==4)
//				{				
//					Tj_A=0;
//					Tj_B=0;
//				}
				
				
				
				}
			}
				
				//////
				
			}
			
			
            break;
        default:
            break;
    }
	
}

uint8_t cntzz=0;
void UART_0_INST_IRQHandler()
{
    switch (DL_UART_Main_getPendingInterrupt(UART_0_INST)) {
    /*! UART interrupt index for UART receive */
    case DL_UART_MAIN_IIDX_RX: 
        gEchoData = DL_UART_Main_receiveData(UART_0_INST);
				if(gEchoData<128)
					NOW_black=gEchoData;
				if (gEchoData>128)
					NOW_black_cnt=gEchoData-128;			
        break;
    default:
        break;
    }
}


int fputc(int c, FILE* stream)
{
    DL_UART_Main_transmitDataBlocking(UART_0_INST, c);
    return c;
}
 
int fputs(const char* restrict s, FILE* restrict stream)
{
    uint16_t i, len;
    len = strlen(s);
    for(i=0; i<len; i++)
    {
        DL_UART_Main_transmitDataBlocking(UART_0_INST, s[i]);
    }
    return len;
}
