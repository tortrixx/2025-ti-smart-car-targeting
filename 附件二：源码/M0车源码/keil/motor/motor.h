#ifndef MOTOR
#define MOTOR
#include "ti_msp_dl_config.h"
extern int flag;
extern float v_A,v_B;
extern float Tv_A,Tv_B;

extern float Tj_A,Tj_B;
extern float Tl_A,Tl_B;
extern float Tz_A,Tz_B;

void motor_init(void);
void motor_a(int pwm);
void motor_b(int pwm);
void motor_handle();
uint8_t jiasuA(float startva,float Tva,int starttim,int overtim,int tim);
uint8_t jiasuB(float startva,float Tva,int starttim,int overtim,int tim);
void findli(uint8_t T,uint8_t now);
void go_zhijiao(float T_V,float maix_cV,float now_angio,float T_angio);
#endif