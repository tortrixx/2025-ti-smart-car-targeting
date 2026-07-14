#ifndef __ENCODER_H_
#define __ENCODER_H_


#define M1_Encoder_Ti1 DL_GPIO_PIN_2
#define M1_Encoder_Ti2 DL_GPIO_PIN_3

#define M2_Encoder_Ti1 DL_GPIO_PIN_0
#define M2_Encoder_Ti2 DL_GPIO_PIN_1
void EXTI_Init(void);
extern int encoder1_num, encoder2_num;

#endif
