#include "encoder.h"
#include "ti_msp_dl_config.h"

int encoder1_num = 0;
int encoder2_num = 0;
uint32_t M1_Enter_Is = 0;
uint32_t M2_Enter_Is = 0;
//四倍频
void EXTI_Init(void)
{
	NVIC_EnableIRQ(GPIOB_INT_IRQn);
}

void GROUP1_IRQHandler(void)
{
	
	switch(DL_Interrupt_getPendingGroup(DL_INTERRUPT_GROUP_1))
	{
		case DL_INTERRUPT_GROUP1_IIDX_GPIOB:
		{
			M1_Enter_Is = DL_GPIO_getEnabledInterruptStatus(GPIOB,M1_Encoder_Ti1 | M1_Encoder_Ti2);
			M2_Enter_Is = DL_GPIO_getEnabledInterruptStatus(GPIOB,M2_Encoder_Ti1 | M2_Encoder_Ti2);
			if((M1_Enter_Is) == M1_Encoder_Ti1)
			{
				M1_Enter_Is = 0;
				if(DL_GPIO_readPins(GPIOB, M1_Encoder_Ti1) == 1)//TI1上升沿
				{
					if(DL_GPIO_readPins(GPIOB, M1_Encoder_Ti2) == 1)//TI2高电平
					{
						encoder1_num --;
					}
					else if(DL_GPIO_readPins(GPIOB, M1_Encoder_Ti2) == 0)//TI2低电平
					{
						encoder1_num ++;
					}
				}
				else if(DL_GPIO_readPins(GPIOB, M1_Encoder_Ti1) == 0)//TI1下降沿
				{
					if(DL_GPIO_readPins(GPIOB, M1_Encoder_Ti2) == 1)//TI2高电平
					{
						encoder1_num ++;
					}
					else if(DL_GPIO_readPins(GPIOB, M1_Encoder_Ti2) == 0)//TI2低电平
					{
						encoder1_num --;
					}
				}
			}
			else if((M1_Enter_Is) == M1_Encoder_Ti2)
			{
				M1_Enter_Is = 0;
				if(DL_GPIO_readPins(GPIOB, M1_Encoder_Ti2) == 1)//TI2上升沿
				{
					if(DL_GPIO_readPins(GPIOB, M1_Encoder_Ti1) == 1)//TI1高电平
					{
						encoder1_num ++;
					}
					else if(DL_GPIO_readPins(GPIOB, M1_Encoder_Ti1) == 0)//TI1低电平
					{
						encoder1_num --;
					}
				}
				else if(DL_GPIO_readPins(GPIOB, M1_Encoder_Ti2) == 0)//TI2下降沿
				{
					if(DL_GPIO_readPins(GPIOB, M1_Encoder_Ti1) == 1)//TI1高电平
					{
						encoder1_num --;
					}
					else if(DL_GPIO_readPins(GPIOB, M1_Encoder_Ti1) == 0)//TI1低电平
					{
						encoder1_num ++;
					}
				}
			}
			if((M2_Enter_Is) == M2_Encoder_Ti1)
			{
				M2_Enter_Is = 0;
				if(DL_GPIO_readPins(GPIOB, M2_Encoder_Ti1) == 1)//TI1上升沿
				{
					if(DL_GPIO_readPins(GPIOB, M2_Encoder_Ti2) == 1)//TI2高电平
					{
						encoder2_num --;
					}
					else if(DL_GPIO_readPins(GPIOB, M2_Encoder_Ti2) == 0)//TI2低电平
					{
						encoder2_num ++;
					}
				}
				else if(DL_GPIO_readPins(GPIOB, M2_Encoder_Ti1) == 0)//TI1下降沿
				{
					if(DL_GPIO_readPins(GPIOB, M2_Encoder_Ti2) == 1)//TI2高电平
					{
						encoder2_num ++;
					}
					else if(DL_GPIO_readPins(GPIOB, M2_Encoder_Ti2) == 0)//TI2低电平
					{
						encoder2_num --;
					}
				}
			}
			else if((M2_Enter_Is) == M2_Encoder_Ti2)
			{
				M2_Enter_Is = 0;
				if(DL_GPIO_readPins(GPIOB, M2_Encoder_Ti2) == 1)//TI2上升沿
				{
					if(DL_GPIO_readPins(GPIOB, M2_Encoder_Ti1) == 1)//TI1高电平
					{
						encoder2_num ++;
					}
					else if(DL_GPIO_readPins(GPIOB, M2_Encoder_Ti1) == 0)//TI1低电平
					{
						encoder2_num --;
					}
				}
				else if(DL_GPIO_readPins(GPIOB, M2_Encoder_Ti2) == 0)//TI2下降沿
				{
					if(DL_GPIO_readPins(GPIOB, M2_Encoder_Ti1) == 1)//TI1高电平
					{
						encoder2_num --;
					}
					else if(DL_GPIO_readPins(GPIOB, M2_Encoder_Ti1) == 0)//TI1低电平
					{
						encoder2_num ++;
					}
				}
			}
		}break;
		
		default:break;
	}
	M1_Enter_Is = 0;
	M2_Enter_Is = 0;
//	if(encoder1_num>9999||encoder1_num<-9999) encoder1_num=0;
//	else if(encoder2_num>9999||encoder2_num<-9999) encoder2_num=0;
	DL_GPIO_clearInterruptStatus(GPIOB,M1_Encoder_Ti1 | M1_Encoder_Ti2 | M2_Encoder_Ti1 | M2_Encoder_Ti2);
}
