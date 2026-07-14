#include "motor.h"

#include "encoder.h"
void motor_init(void)
{
	DL_TimerA_startCounter(TIMA0);
	DL_TimerA_setCaptureCompareValue(TIMA0,2000, DL_TIMER_CC_0_INDEX);
	DL_TimerA_setCaptureCompareValue(TIMA0,2000, DL_TIMER_CC_1_INDEX);
	DL_GPIO_setPins(GPIOA, DL_GPIO_PIN_31);
	DL_GPIO_setPins(GPIOA, DL_GPIO_PIN_30);
	DL_GPIO_setPins(GPIOA, DL_GPIO_PIN_29);
	DL_GPIO_setPins(GPIOA, DL_GPIO_PIN_28);	
}

float ceh_A=0,ceh_B=0;
float Tv_A=0,Tv_B=0;

float Tj_A=0,Tj_B=0;
float Tl_A=0,Tl_B=0;
float Tz_A=0,Tz_B=0;


float v_A=0,v_B=0;
float error_v_A=0,error_v_B=0;
float last_error_v_A=0,last_error_v_B=0;
float pwmA=0,pwmB=0;

float pid_out_VA=0,pid_out_VB=0;

float lz_A=0,lz_B=0;
float fffff=0;









void motor_b(int pwm)
{
	if(pwm<0)
	{
		DL_GPIO_clearPins(GPIOA, DL_GPIO_PIN_30);
		DL_GPIO_setPins(GPIOA,DL_GPIO_PIN_31);
		
		pwm = -pwm;
	}
	else
	{
		DL_GPIO_clearPins(GPIOA, DL_GPIO_PIN_31);
		DL_GPIO_setPins(GPIOA, DL_GPIO_PIN_30);
	}
	DL_TimerA_setCaptureCompareValue(TIMA0,pwm, DL_TIMER_CC_1_INDEX);
}

void motor_a(int pwm)
{
	if(pwm<0)
	{
		DL_GPIO_clearPins(GPIOA, DL_GPIO_PIN_28);
		DL_GPIO_setPins(GPIOA, DL_GPIO_PIN_29);
		pwm = -pwm;
	}
	else
	{
		DL_GPIO_clearPins(GPIOA, DL_GPIO_PIN_29);
		DL_GPIO_setPins(GPIOA,DL_GPIO_PIN_28);
	}
	DL_TimerA_setCaptureCompareValue(TIMA0,pwm, DL_TIMER_CC_0_INDEX);
}



void pid_VA(void)
{
	last_error_v_A=error_v_A;
	error_v_A=Tv_A-v_A;
	pid_out_VA=150*(error_v_A-last_error_v_A)+30*error_v_A;
	pwmA=pwmA-pid_out_VA;
	 if(pwmA>9999)
	 	pwmA=9999;
     else if(pwmA<-9999)
	 	pwmA=-9999;
	motor_a((int)pwmA);
}

void pid_VB(void)//300 75 100hz  200 95 200hz 100 100 500hz
{
	last_error_v_B=error_v_B;
	error_v_B=Tv_B-v_B;
	pid_out_VB= 150*(error_v_B-last_error_v_B)+30*error_v_B;
	pwmB=pwmB-pid_out_VB;
	 if(pwmB>9999)
	 	pwmB=9999;
	 else if(pwmB<-9999)
	 	pwmB=-9999;
	 

	if(Tv_B*pwmB<0)
	{
//	  pwmB=0;
	}
	motor_b((int)pwmB);
}

void motor_handle()
{
		v_A=-(encoder1_num-lz_A)*1.6f;  //80*100/5000
		v_B=-(encoder2_num-lz_B)*1.6f;
		lz_A=encoder1_num;
		lz_B=encoder2_num;
		Tv_A=Tj_A+Tl_A+Tz_A,Tv_B=Tj_B+Tl_B+Tz_B;
		pid_VA();
		pid_VB();
}


float lle_e=0,le_e;
float le_out;
void findli(uint8_t T,uint8_t now)
{
	le_e=now-T;
	if(le_e>10||le_e<-10)
	{
	if(le_e>10)
		le_e=le_e-10;
	else
		le_e=le_e+10;
	le_out=0.06*le_e-0.015*(lle_e-le_e);
	lle_e=now-T;
	}
	else
		le_out=0;
		Tl_A=-le_out;
		Tl_B=le_out;
}

uint8_t jiasuA(float startva,float Tva,int starttim,int overtim,int tim)
{
	float ctim=tim-starttim;
	float ztim=overtim-starttim;
	float	k=ctim/ztim;
	if(k<=1)
	Tj_A=startva+Tva*ctim/ztim;
	else
		return 0;
	return 1;
}

uint8_t jiasuB(float startvb,float Tvb,int starttim,int overtim,int tim)
{
	float ctim=tim-starttim;
	float ztim=overtim-starttim;
	float	k=ctim/ztim;
	if(k<=1)
	Tj_B=startvb+Tvb*ctim/ztim;
	else
		return 0;
	return 1;
}

void zhuanwan(int Tea,int Teb,float vamin,float vbmin,float vamax,float vbmax)
{

}




