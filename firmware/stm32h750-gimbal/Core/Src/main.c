/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"
#include "memorymap.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdlib.h>
#include "math.h"
#include "TMC2209.h"
#include "Kalman.h"
#include "Zhangdatou.h"
#include "kalman_twice.h"
#include "stdarg.h"
#include "OLED.h"
#include "wit.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
extern int lookup_table[];

#define CHEB_ORDER 6
#define X_MIN 0.000000f
#define X_MAX 6490.000000f
static const float cheb_coef[CHEB_ORDER+1] = {
    -836.2304769668f,
    -997.8101985323f,
    -13.4824721790f,
    -21.8237562184f,
    8.4930403253f,
    -10.0704451518f,
    6.1022329334f,
};

static inline float cheb_normalize(float x) {
    return (2.0f * x - (X_MAX + X_MIN)) / (X_MAX - X_MIN);
}

float cheb_eval(float x) {
    float xp = cheb_normalize(x);
    float T0 = 1.0f;
    float T1 = xp;
    float y = cheb_coef[0] * T0 + cheb_coef[1] * T1;
    for (int n = 2; n <= CHEB_ORDER; n++) {
        float Tn = 2.0f * xp * T1 - T0;
        y += cheb_coef[n] * Tn;
        T0 = T1;
        T1 = Tn;
    }
    return y-194.586731;
}




#define CHEB_ORDER2 6
#define X_MIN2 0.000000f
#define X_MAX2 6325.000000f
static const float cheb_coef2[CHEB_ORDER2+1] = {
    3055.9000644695f,
    2946.5198651746f,
    63.1994350443f,
    -198.8765504428f,
    -33.8485973053f,
    -131.5738849236f,
    -28.5150453190f,
};

static inline float cheb_normalize2(float x) {
    return (2.0f * x - (X_MAX2 + X_MIN2)) / (X_MAX2 - X_MIN2);
}

float cheb_eval2(float x) {
    float xp = cheb_normalize2(x);
    float T0 = 1.0f;
    float T1 = xp;
    float y = cheb_coef2[0] * T0 + cheb_coef2[1] * T1;
    for (int n = 2; n <= CHEB_ORDER2; n++) {
        float Tn = 2.0f * xp * T1 - T0;
        y += cheb_coef2[n] * Tn;
        T0 = T1;
        T1 = Tn;
    }
    return y-440.666595;
}



#include <math.h>

// Scaler parameters
float scaler_x_mean = 3162.5f;
float scaler_x_scale = 1826.1588786302248f;
float scaler_y_mean = 3037.9078406576036f;
float scaler_y_scale = 1788.344798268679f;

// Layer 0: 1 -> 10
float weights_0[1][10] = {
    {0.389808, -0.220694, -0.060530, 0.362304, -0.392653, -0.386696, -0.525085, 0.287447, 0.388318, -0.323335}
};
float biases_0[10] = {-0.154673, -0.076658, -0.136058, -0.081717, 0.143183, 0.106359, -0.078212, 0.059661, 0.084437, 0.027147};

// Layer 1: 10 -> 1
float weights_1[10][1] = {
    {0.445674},
    {-0.105056},
    {-0.673103},
    {0.569739},
    {-0.631793},
    {-0.440583},
    {0.071213},
    {-0.030064},
    {-0.067304},
    {-0.882389}
};
float biases_1[1] = {0.152199};

// tanh activation - 使用查表优化
#define TANH_TABLE_SIZE 2048
#define TANH_INPUT_RANGE 4.0f
static float tanh_table[TANH_TABLE_SIZE];

float fast_tanh(float x) {
    float abs_x = fabsf(x);
    if (abs_x >= TANH_INPUT_RANGE) return (x > 0) ? 1.0f : -1.0f;
    
    float scaled_x = (abs_x * TANH_TABLE_SIZE) / TANH_INPUT_RANGE;
    int index = (int)scaled_x;
    float frac = scaled_x - index;
    
    float y = tanh_table[index] + 
              (index < TANH_TABLE_SIZE-1 ? 
               (tanh_table[index+1] - tanh_table[index]) * frac : 0);
               
    return (x > 0) ? y : -y;
}

// 推理函数
float nn_predict(float input) {
    float x = (input - scaler_x_mean) / scaler_x_scale;
    float layer_0_out[10];
    
    // Layer 0
    for (int j = 0; j < 10; j++) {
        layer_0_out[j] = biases_0[j];
        layer_0_out[j] += weights_0[0][j] * x;
        layer_0_out[j] = fast_tanh(layer_0_out[j]);
    }
    
    // Layer 1
    float output = biases_1[0];
    for (int k = 0; k < 10; k++) {
        output += weights_1[k][0] * layer_0_out[k];
    }
    
    return output * scaler_y_scale + scaler_y_mean;
}

// 初始化tanh查找表
__attribute__((constructor))
void init_tanh_table() {
    for (int i = 0; i < TANH_TABLE_SIZE; i++) {
        float x = (float)i * TANH_INPUT_RANGE / TANH_TABLE_SIZE;
        tanh_table[i] = tanhf(x);
    }
}





void usart_printf(char *format,...)           //串口格式字符串输出
{
    int i =0;
    char String[100];         //定义输出字符串
    va_list arg;             //定义一个参数列表变量va_list是一个类型名，arg是变量名
    va_start(arg,format);     //从format位置开始接收参数表放在arg里面
    
    //sprintf打印位置是String，格式化字符串是format，参数表是arg，对于封装格式sprintf要改成vsprintf
    vsprintf(String,format,arg);
    va_end(arg);             //释放参数表
    while(1)
    {
        if(String[i] == 0)
        {
            return;
        }
        HAL_UART_Transmit(&huart8,(uint8_t *)&String[i], 1, 0xffff);
        i++;
    }    
}



//按键外部中断
uint8_t PC4=0;
uint8_t PB0=0;
uint8_t PB2=0;

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  switch (GPIO_Pin)
  {
  case GPIO_PIN_0:
    PB0=1;
    /* code */
    break;
  case GPIO_PIN_2:
    PB2=1;
    break;
  case GPIO_PIN_4:
    PC4=1;
    break;

  default:
    break;
  }
}


uint8_t arrow = 0;
uint8_t menu_state()   //菜单
{
  OLED_ShowStr(2,0,(uint8_t*)"track",16);
  OLED_ShowStr(2,2,(uint8_t*)"rightscan",16);
  OLED_ShowStr(2,4,(uint8_t*)"leftscan",16);
  OLED_ShowStr(2,6,(uint8_t*)"circle",16);
  if(PB0 ==1)
  {
    arrow--;
    OLED_FullyClear();
    OLED_ShowStr(100,arrow*2,(uint8_t*)"<",16);
    PB0 =0;
  }
  else if(PB2 ==1)
  {
    arrow++;
    OLED_FullyClear();
    OLED_ShowStr(100,arrow*2,(uint8_t*)"<",16);
    PB2=0;
  }
  else if(PC4 == 1)
  {
    HAL_GPIO_WritePin(GPIOC,GPIO_PIN_10,0);
    zhang_motor_enable_x(1);
  HAL_Delay(100);
  zhang_motor_enable_y(1);
  HAL_Delay(100);
    PC4 =0;
HAL_GPIO_WritePin(GPIOC,GPIO_PIN_10,1);
    
return arrow+1;
  }

  return 0;
}



uint32_t getMicros();

/**
 * pid运算函数
 */
float pid_calculate(pid_handler *pid,float err)
{
	// if(fabs(err)>pid->err_limit)
  // {
  //   return pid->OUT;
  // }

  float POUT = err*(pid->P);
  uint32_t now_time = getMicros();
  float difftime = (now_time - pid->last_time)*1e-6f;
  if (difftime < 1e-6f) difftime = 1e-6f;

  pid->last_time = now_time;
  
  
  //pid->integral += difftime*(err+pid->previous)*0.5f*(pid->I);  //梯形积分

  
  pid->integral += difftime*(err);
  pid->integral = restrict(pid->integral,pid->inte_limit[0],pid->inte_limit[1]);

  float IOUT = pid->integral*(pid->I);



  pid->integral2 +=difftime*pid->integral;  //二次积分
  pid->integral2 = restrict(pid->integral,pid->inte_limit[0],pid->inte_limit[1]);

  //float I2OUT = pid->integral2*pid->I2;
  float DOUT=0;
  if((pid->d_state)==0)
  {
    DOUT = (err - pid->previous)*(pid->D)/difftime;   //pid周期微分项计算
  }
  else
  {
    DOUT = (pid->de)*pid->D;   //外部更新微分项
  }
  


  if((pid->previous)*err<=0)      //积分清零
  {
    pid->integral=0;
    pid->integral2=0;
  }
  pid->previous = err;



  pid->OUT += (POUT+IOUT+DOUT)*difftime;
  //pid->OUT = restrict(pid->OUT,pid->out_limit[0],pid->out_limit[1]);
  return pid->OUT;
}

float LowPassFilterRun(LowPassFilter *Filter,float x)   //低通滤波器
{
    unsigned long timestamp = getMicros();
    float dt = (timestamp - Filter->timestamp_prev)*1e-6f;

    if (dt < 0.0f ) dt = 1e-3f;
    else if(dt > 0.3f) {
        Filter->y_prev = x;
        Filter->timestamp_prev = timestamp;
        return x;
    }

    float alpha = (Filter->Tf)/((Filter->Tf) + dt);
    float y = alpha*(Filter->y_prev) + (1.0f - alpha)*x;
    Filter->y_prev = y;
    Filter->timestamp_prev = timestamp;
    return y;
}

/**
 * pid增量式(废弃)
 */
float pid_calculate_increment(pid_handler *pid,float err)
{
	uint32_t now_time = getMicros();
  float difftime = (now_time - pid->last_time)*1e-6f;
  if (difftime < 1e-6f) difftime = 1e-6f;
  pid->last_time = now_time;

  float POUT = (pid->P)*(err - pid->previous);
  float IOUT = (pid->I)*err*difftime;
  float DOUT = (pid->D)*(err - 2*(pid->previous)+pid->last_previous)/difftime;



  pid->last_previous = pid->previous;
  pid->previous = err;

  pid->OUT += POUT+IOUT+DOUT;
  //pid->OUT = restrict(pid->OUT,pid->out_limit[0],pid->out_limit[1]);
  return pid->OUT;
}


/**
 * pid结构体复位
 */
void pid_reset(pid_handler *pid) {
    pid->integral = 0;
    pid->integral2 = 0;
    pid->previous = 0;
    pid->last_previous = 0;
    pid->OUT = 0;
    pid->last_time = getMicros();
}






uint32_t getMicros()   //获取微秒
{
  /* Ensure COUNTFLAG is reset by reading SysTick control and status register */
   (void)SysTick->CTRL;
  uint32_t m = HAL_GetTick();
  const uint32_t tms = SysTick->LOAD + 1;
  __IO uint32_t u = tms - SysTick->VAL;
  if (SysTick->CTRL &SysTick_CTRL_COUNTFLAG_Msk) {
    m = HAL_GetTick();
    u = tms - SysTick->VAL;
  }
  return (m * 1000 + (u * 1000) / tms);
}

void delay_us(uint32_t time)   //微秒延时
{
  uint32_t tick = getMicros();
  while ((getMicros()-tick)<time);
}



int pul_speed = 10;   //脉冲间隔(us)，运行插补算法时通过该值调节响应速度

void generate_pul_2()
{
  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,1);
  delay_us(pul_speed);
  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,0);
  delay_us(pul_speed);
}


void generate_pul_1()
{
  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_6,1);
  delay_us(pul_speed);
  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_6,0);
  delay_us(pul_speed);
}
void set_dir_2(uint8_t dir)
{
  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_5,dir);
}
void set_dir_1(uint8_t dir)
{
  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_7,!dir);
}

/**
 * x轴步进
 */
void step_motor1(uint8_t dir) {
    set_dir_1(dir);
    generate_pul_1();
}
/**
 * y轴步进
 */
void step_motor2(uint8_t dir) {
    set_dir_2(dir);
    generate_pul_2();
}



// int current_x=0;
// int current_y=0;

//废弃
// void bresenham_line(int x0, int y0, int x1, int y1)
// {
//     int dx = abs(x1 - x0);
//     int dy = abs(y1 - y0);
//     int sx = (x0 < x1) ? 1 : -1;
//     int sy = (y0 < y1) ? 1 : -1;
//     int err = dx - dy;

//     if(sx ==1)
//     {
//       set_dir_2(1);
//     }
//     else
//     {
//       set_dir_2(0);
//     }
    
//     if(sy ==1)
//     {
//       set_dir_1(1);
//     }
//     else
//     {
//       set_dir_1(0);
//     }
//     while (1) {
        

//         if (x0 == x1 && y0 == y1)
//             break;

//         int e2 = 2 * err;

//         if (e2 > -dy) {
//             err -= dy;
//             x0 += sx;
//             generate_pul_2();
//         }

//         if (e2 < dx) {
//             err += dx;
//             y0 += sy;
//             generate_pul_1();
//         }
//     }
// }
#define DIR_CW    1     // 顺时针方向
#define DIR_CCW   0     // 逆时针方向
// 改进的Bresenham直线插补算法
void bresenham_line(int x0, int y0, int x1, int y1) 
{
    if(x0 == x1 && y0 == y1) {
        return;  // 如果重合直接返回，不执行任何动作
    }
    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);
    int dir_x = x0 < x1 ? DIR_CW : DIR_CCW;
    int dir_y = y0 < y1 ? DIR_CW : DIR_CCW;
    
    // 确定主轴和从轴
    int is_x_major = (dx >= dy);
    int major_steps = is_x_major ? dx : dy;
    int minor_steps = is_x_major ? dy : dx;
    
    // 初始化误差
    int error = major_steps / 2;
    
    // 记录当前位置
    int x = x0;
    int y = y0;       
    
    // 执行插补
    for(int i = 0; i <= major_steps; i++) {
        // 主轴总是需要步进
        if(is_x_major) {
            step_motor2(dir_x);
            x += (dir_x == DIR_CW) ? 1 : -1;
        } else {
            step_motor1(dir_y);
            y += (dir_y == DIR_CW) ? 1 : -1;
        }
        
        // 判断从轴是否需要步进
        error -= minor_steps;
        if(error < 0) {
            if(is_x_major) {
                step_motor1(dir_y);
                y += (dir_y == DIR_CW) ? 1 : -1;
            } else {
                step_motor2(dir_x);
                x += (dir_x == DIR_CW) ? 1 : -1;
            }
            error += major_steps;
        }
        
        
    }
}


float current_x = 0;   //当前x脉冲计数
float current_y = 0;   //当前y脉冲计数


//绝对脉冲插补控制,阻塞式步进，返回后脉冲计数已达到目标位置
void move_to_position(float target_x, float target_y) 
{
    bresenham_line(current_x, current_y, (int)target_x, (int)target_y);
    current_x = target_x;
    current_y = target_y;
}

//相对脉冲插补控制(废弃)
void move_to_pul(float target_pulx, float target_puly)  
{
  bresenham_line(0, 0, (int)target_pulx, (int)target_puly);
} 




float theta=0;
float r = 100;
float ex=0,ey=0;

void draw_circle()
{
     ex =r* cos(theta);
     ey =r* sin(theta);
    theta +=0.001;
    move_to_position(ex,ey);
    // HAL_Delay(10);
}



uint8_t stagex =1;
void draw_cos()   //正弦绘制
{
    // ex =r* cos(theta);
     ey =r* sin(theta);


     if(stagex)
     {
        ex+=0.02;
        if(ex>800)
        {
          stagex =0;
        }
     }
     else
     {
      ex-=0.02;
        if(ex<0)
        {
          stagex =1;
        }
     }
    theta +=0.0005;
    move_to_position(ex,ey);
    // HAL_Delay(10);
}


void laser_enable(uint8_t i)   //开关激光
{
  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_6,i);
}



//extern const int (*all_contours[])[2];  //轮廓集合
//extern const int contour_lengths[]; //轮廓点数量
//uint8_t st =0;  //判定绘制过程或移动过程
//void draw_counter()   //轮廓绘制
//{
//  pul_speed = 500;
//    for (int i = 0; i < 101; i++) {
//        const int (*path)[2] = all_contours[i];
//        int len = contour_lengths[i];
//        for (int j = 0; j < len; j++) {
//            int x = path[j][0]*2;
//            int y = path[j][1]*2;
//            if(st==1)
//            {
//              laser_enable(1);
//            }
//            move_to_position(x, y);  
//            st = 1;
//        }
//        st = 0;
//        laser_enable(0);
//    }
//}



/**
 * 串口收发区
 */

uint8_t Maixcam_RX_stage=0;
uint8_t Maixcam_RX_data;
uint8_t Maixcam_RX_fifo[2000] = {0};
int Maixcam_RX_count =0;

void maixcam_RX_finish();    //maixcam数据包接收完成回调
void Maixcam_RX_callback()   //maixcam串口回调
{
  switch (Maixcam_RX_stage)
  {
  case 0:
    if(Maixcam_RX_data == 0xAA)
    {
      Maixcam_RX_stage = 1;
    }
    break;
  case 1:
    if(Maixcam_RX_data == 0x12)
    {
      Maixcam_RX_stage = 2;
    }
    else
    {
      Maixcam_RX_stage = 0;
    }
    break;
  case 2:
    Maixcam_RX_fifo[Maixcam_RX_count++] = Maixcam_RX_data;
    if(Maixcam_RX_data == 0x89)
    {
      
      Maixcam_RX_stage =3;
    }
    break;
  case 3:
    if(Maixcam_RX_data == 0x78)
    {
      Maixcam_RX_stage =0;
      maixcam_RX_finish();
      
      Maixcam_RX_count=0;
    }
    else
    {
      Maixcam_RX_fifo[Maixcam_RX_count++] = Maixcam_RX_data;
      if(Maixcam_RX_data != 0x89)
      {Maixcam_RX_stage =2;}
    
    }
    break;

  }
HAL_UART_Receive_IT(&huart1,&Maixcam_RX_data,1);
}

//g3507编码器通讯

uint8_t g3507_RX_data=0;
uint8_t g3507_RX_stage=0;
uint8_t g3507_RX_fifo[20] = {0};
int g3507_RX_count =0;


void g3507_RX_finish();
void m0g3507_RX_callback()
{
  switch (g3507_RX_stage)
  {
  case 0:
    if(g3507_RX_data == 0xCA)
    {
      g3507_RX_stage = 1;
    }
    break;
  case 1:
    if(g3507_RX_data == 0xAC)
    {
      g3507_RX_stage = 2;
    }
    else
    {
      g3507_RX_stage = 0;
    }
    break;
  case 2:
    g3507_RX_fifo[g3507_RX_count++] = g3507_RX_data;
    if(g3507_RX_data == 0x12)
    {
      
      g3507_RX_stage =3;
    }
    break;
  case 3:
    if(g3507_RX_data == 0x23)
    {
      g3507_RX_stage =0;
      g3507_RX_finish();
      
      g3507_RX_count=0;
    }
    else
    {
      g3507_RX_fifo[g3507_RX_count++] = g3507_RX_data;
      if(g3507_RX_data != 0x89)
      {g3507_RX_stage =2;}
    
    }
    break;

  }

  HAL_UART_Receive_IT(&huart4,&g3507_RX_data,1);
}


int nxc1=0;
int nyc1=0;

extern int nxc;  //当前坐标
extern int nyc; //当前坐标


KFPTypeS_Struct kfpVarx = 
{
   10.00, //P估算协方差. 初始化值为 0.02
   0, //G卡尔曼增益. 初始化值为 0
   0.6, //Q过程噪声协方差,Q增大，动态响应变快，收敛稳定性变坏. 初始化值为 0.001
   0.2, //R测量噪声协方差,R增大，动态响应变慢，收敛稳定性变好. 初始化值为 1
   0 //卡尔曼滤波器输出. 初始化值为 0
};

KFPTypeS_Struct kfpVary = 
{
   10.000, //P估算协方差. 初始化值为 0.02
   0, //G卡尔曼增益. 初始化值为 0
   0.6, //Q过程噪声协方差,Q增大，动态响应变快，收敛稳定性变坏. 初始化值为 0.001
   0.2, //R测量噪声协方差,R增大，动态响应变慢，收敛稳定性变好. 初始化值为 1
   0 //卡尔曼滤波器输出. 初始化值为 0
};

extern int exc;  //激光x坐标
extern int eyc;  //激光y坐标
int ridus=0;



extern int nspeedx;   //目标移动速度
extern int last_lx; //上次位置

extern int nspeedy;   //目标移动速度
extern int last_ly; //上次位置

float* kalman_x_redata;
float* kalman_y_redata;




// pid_handler pidx;
// pid_handler pidy;

void maixcam_RX_finish()  //maixcam数据包接收完成回调
{
  nxc1 =   *(int*)&Maixcam_RX_fifo[0];
  nyc1 =   *(int*)&Maixcam_RX_fifo[4];
  exc = *(int*)&Maixcam_RX_fifo[8];
  eyc = *(int*)&Maixcam_RX_fifo[12];
  ridus = *(int*)&Maixcam_RX_fifo[16];
  // exc = *(int*)&Maixcam_RX_fifo[8];
  // eyc = *(int*)&Maixcam_RX_fifo[12];


  // kalman_x_redata = kalman_calcu_x(nxc1);
  // kalman_y_redata = kalman_calcu_y(nyc1);
  // nxc = kalman_x_redata[0];
  // nyc = kalman_y_redata[0];


  // nxc = nxc1;
  // nyc = nyc1;

  // uint32_t ntime = getMicros();
  // float difftime = ntime-last_us_tick;
  // last_us_tick = ntime;
  // pidx.de = (nxc - last_lx)/difftime;
  // last_lx = nxc;

  // pidy.de = (nyc - last_ly)/difftime;
  // last_ly = nyc;


//usart_printf("%d,%d,%d,%d\n",nxc1,nyc1,nxc,nyc);
}

int enconter =0;
uint8_t turn=0;
uint8_t turn_count=0;

void g3507_RX_finish()
{
  enconter = *(int*)(&g3507_RX_fifo[0]);
  turn = *(uint8_t*)(&g3507_RX_fifo[4]);
  turn_count = *(uint8_t*)(&g3507_RX_fifo[5]);
}


//陀螺仪区
extern SensorData wit_data;

//不行的官方库
// float lastyaw = 0;

// 	float fAcc[3], fGyro[3], fAngle[3];
// 	int i;
// #define ACC_UPDATE		0x01
// #define GYRO_UPDATE		0x02
// #define ANGLE_UPDATE	0x04
// #define MAG_UPDATE		0x08
// #define READ_UPDATE		0x80
// static volatile char s_cDataUpdate1 = 0,s_cDataUpdate2 = 0,s_cDataUpdate3 = 0,s_cDataUpdate4 = 0, s_cCmd = 0xff;

// extern uint8_t ucRegIndex;
// extern uint16_t usRegDataBuff[4];
// extern uint32_t uiRegDataLen;
// uint8_t JY901_RX_data=0;
// void jy901_callback()
// {
//   WitSerialDataIn(JY901_RX_data);
// HAL_UART_Receive_IT(&huart6,&JY901_RX_data,1);

// }
// static void JY901_SensorUartSend(uint8_t *p_data, uint32_t uiSize)
// {
// 	HAL_UART_Transmit(&huart6,p_data,uiSize,0xff);
// }
// static void JY901_SensorDataUpdata(uint32_t uiReg, uint32_t uiRegNum)
// {
// 	int i;
//     for(i = 0; i < uiRegNum; i++)
//     {
//         switch(uiReg)
//         {
// //            case AX:
// //            case AY:
//             case AZ:
// 				s_cDataUpdate1 |= ACC_UPDATE;
//             break;
// //            case GX:
// //            case GY:
//             case GZ:
// 				s_cDataUpdate2 |= GYRO_UPDATE;
//             break;
// //            case HX:
// //            case HY:
//             case HZ:
// 				s_cDataUpdate4 |= MAG_UPDATE;
//             break;
// //            case Roll:
// //            case Pitch:
//             case Yaw:
// 				s_cDataUpdate3 |= ANGLE_UPDATE;
//             break;
// //            default:
// //				s_cDataUpdate1 |= READ_UPDATE;
// //			break;
//         }
// 		uiReg++;
//     }
// }
//陀螺仪区end


/**
 * 串口收发区end
 */

/**
 * pid控制区
 */

uint8_t control_state=0;//控制选项  1:跟踪控制 ,2:右扫描,3:左扫描 .4:角度控制,0菜单
uint8_t laser_control = 1; // 0:闭环激光控制1：开环激光控制
float init_angle_pid=0;
float init_acc_pid=0;




int nspeedx =0;
int last_lx=0;
int nspeedy =0;
int last_ly=0;


int nxc=0;  //当前x坐标
int nyc=0;  //当前y坐标

int exc=210;  //期望x坐标
int eyc=225;  //期望y坐标

int oexc=195;//开环期望坐标
int oeyc=235;  




float exangle = 0; //期望角度，角度环




//参数结构体



pid_handler pidx = //x位置环
{
  .P = 40,
  .I = 40,
  .D = 6,
  .I2 = 0,
  .out_limit = {-10000,10000},
  .inte_limit = {-1000,1000},
  .d_state=0,     //外部微分
  .err_limit = 30,
  .acc_Feed =0    //外部前馈
};

pid_handler pidx1 = //x位置环,近段
{
  .P = 50,
  .I = 80,
  .D = 2,
  .I2 = 4,
  .out_limit = {-10000,10000},
  .inte_limit = {-1000,1000},
  .d_state=0,     //外部微分
  .err_limit = 30,
  .acc_Feed =0    //外部前馈
};

pid_handler * pidxstr =&pidx ;


pid_handler pidy = //y位置环
{
  .P = 25,
  .I = 20,
  .D = 4,
  .I2 =0,
  .out_limit = {-10000,10000},
  .inte_limit = {-5000,5000},
  .d_state=0,
  .err_limit = 30,
  .acc_Feed =0
};


pid_handler pidAx = //角度环
{
  .P = 0,
  .I = 0,
  .D = 0,
  .I2 = 0,
  .out_limit = {-10000,10000},
  .inte_limit = {-5000,5000},
  .d_state =1,
  .err_limit = 30,
  .acc_Feed =0
};

float compensate = 0.07;
float maxcompensate = 0.145;
float mincompensate = 0.07;

float angle1_pul =8148.733086305041191366848684672;//弧度补偿
float angle_pul=142.2222222222222; //角度补偿系数
float acc_pul =0;//加速度补偿系数，没使用
float dis_para[4] = {
  -3726,-10934,-3000,3720
}; //编码器位置角度补偿距离系数


float theta1=0;
float r1 = 100;
float excir = 0;
float eycir = 0;

void draw_circle_1()
{
     excir =ridus* cos(-theta1) +nxc;
     eycir =ridus* sin(-theta1) +nyc;
    //theta1 +=0.00001;
    theta1 = (enconter/26800.0f)*2*3.1415926;

    //move_to_position(ex,ey);
    // HAL_Delay(10);
}



float a1=0;  //pid输出1
float a2=0;  //pid输出2
float an1=0;  //角度补偿输出
float enn1=0; //编码器位置角度补偿输出





static uint8_t Tfirstrun=1; //pid初始运行标志位


uint8_t state_qu=0;


//debug
float pxerr =0;
float therr=0;
//debug


uint64_t tick_angle_buchang=0;
uint64_t tick_kalman=0;

uint8_t circle_state=0;

extern float facc;

int ennerr = 0;
int ennout=0;
uint8_t stage=0;
uint8_t c=0;
uint8_t last_c=0;
uint8_t c_change=0;

float feedenn=0;



float enn2=0;

float fitting_calcu_0(int num)
{
  float y = 0.000018*num*num + -0.361413*num;
  return y;
}


float fitting_calcu_1(int num)
{
  //float y =0.000021*num*num + -0.467049*num + 180.342935;
// float y = 2420.123779 * atan(num / -5473.023949) + 177.338986;

 float y = cheb_eval(num);
  return y;
}



float fitting_calcu_2(int num)
{
  //float y =0.000021*num*num + -0.467049*num + 180.342935;
// float y = 2420.123779 * atan(num / -5473.023949) + 177.338986;

 float y = 0.000003*num*num + -0.219962*num;
  return y;
}



float fitting_calcu_3(int num)
{
  //float y =0.000021*num*num + -0.467049*num + 180.342935;
// float y = 2420.123779 * atan(num / -5473.023949) + 177.338986;

 float y = nn_predict(num);
  return y;
}

void enconter_to_angle()  //编码器转换角度
{
  if( (turn_count-1)%4<0)
  {
    return;
  }
  c = (turn_count-1)%4;

  // if(c!= last_c)
  // {
  //   c_change=1;
  // }
  // last_c = c;


  if(turn ==2)
  {
    feedenn=0;
    // ennout = enconter;
    enn2 = enn1;
    stage =1;
    return;
  }

  if(stage==1)
  {
    ennerr = enconter;
    
    stage=0;
  }
  




  if(c ==3)
  {
    if((enconter-ennerr)<1000)
    {
      feedenn = (enconter-ennerr)*-1;
    }
  }
  
  if(dis_para[c]<0)
  {
    enn1 = -1*atan((enconter-ennerr)/(-dis_para[c]))*angle1_pul+enn2+feedenn;
  }
  else
  {
    enn1 = atan((enconter-ennerr)/dis_para[c])*angle1_pul+enn2+feedenn;
  }

  switch (c)
  {
  case 0:
    enn1 = fitting_calcu_0(enconter-ennerr)+enn2+feedenn;
    break;

  case 2:
    enn1 = fitting_calcu_2(enconter-ennerr)+enn2+feedenn;
    break;


  default:
    break;
  }
  

  
  // if(stage ==1)
  // {
  //   ennerr =  enn1 - ennout;
  //   stage =0;
  // }
  // enn1 = enn1 -  ennerr;



//enn1 = atan(enconter/dis_para[3])*angle1_pul;

}


int last_counter=0;

void track_pid_run()    //追踪pid运行
{
  
  if(Tfirstrun)            //判断是否要初始化pid结构体
  {
      //zhang_motor_openloop_y();//设置y轴开环模式
     tick_angle_buchang = getMicros();
    pid_reset(&pidx);
    pid_reset(&pidy);
    pid_reset(&pidx1);
    Tfirstrun =0;
    init_angle_pid= wit_data.yaw;
    init_acc_pid = facc;
  }
  //pidx.de = wit_data.gyro[2];
  
  //pidx.acc_Feed = (facc-init_acc_pid)*acc_pul;   //加速度前馈补偿

  if((getMicros()-tick_kalman)>20000)
  {
    nxc = KalmanFilter(&kfpVarx,nxc1);
    nyc = KalmanFilter(&kfpVary,nyc1);
    tick_kalman = getMicros();
  }



  enconter_to_angle();

  if(laser_control==0) //激光点闭环标志位
  {
  a1 = pid_calculate(&pidx,nxc-exc);
  a2 = pid_calculate(&pidy,nyc-eyc);
  }
  else
  {
  if(circle_state ==1)
  {
    draw_circle_1();
    a1 = pid_calculate(pidxstr,excir-oexc);
    a2 = pid_calculate(&pidy,eycir-oeyc);
  }
  else
  {
    a1 = pid_calculate(pidxstr,nxc-oexc);
    a2 = pid_calculate(&pidy,nyc-oeyc);
  }

    //   if(c_change)
    // {
    //     if(c==3)
    //     {
    //       pidx1.OUT = pidx.OUT;
    //       pidx1.last_time = getMicros();
    //       pidxstr = &pidx1;
    //     }
    //     else if(c==0)
    //     {
    //       pidx.OUT = pidx1.OUT;
    //       pidx.last_time = getMicros();
    //       pidxstr = &pidx;  
    //     }
    //     c_change=0;
    // }
    
  }

  

  // a1 = KalmanFilter(&kfpVarx,a1);
  // a2 = KalmanFilter(&kfpVary,a2);

  // a1 = pid_calculate(&pidx,excir-exc);
  // a2 = pid_calculate(&pidy,eycir-eyc);
  //draw_circle_1();

  if((getMicros()-tick_angle_buchang)>1000)   //1000us运行周期
  {

//     if(fabs(wit_data.gyro[2])>60)   //角速度补偿
//     {
//       compensate = maxcompensate;
//     }
//     else
//     {
//       compensate = mincompensate;
//     }

// an1 += compensate*wit_data.gyro[2];
    //角度补偿
    an1 = angle_pul*(wit_data.yaw-init_angle_pid);

     tick_angle_buchang = getMicros();

  }



  if(state_qu==0)
  {
    move_to_position(a1+an1+enn1,a2);
    //move_to_position(fitting_calcu_3(enconter),a2);
  }
  else
  {
    move_to_position(a1,a2);
  }


  //debug,发送脉冲信息
  
  // if(enconter >= last_counter+1)
  // {
  //   int data = a1+an1+enn1;
  //   int err = enconter - last_counter;
  //   for (int i = 0; i < err; i++)
  //   {
  //     HAL_UART_Transmit(&huart8,(uint8_t*)&data,4,0xffff);
  //   }
    

    
  //   last_counter =enconter;
  // }

  //debug

}







float init_angle=0;
static uint8_t Afirstrun=1; //pid初始运行标志位
void angle_pid_run()        //角度环pid
{
  if(Afirstrun)            //判断是否要初始化pid结构体
  {
    pid_reset(&pidAx);
    Afirstrun =0;
    exangle = wit_data.yaw;
 
    
  }
  pidAx.de = wit_data.gyro[2];
  a1 = pid_calculate(&pidAx,wit_data.yaw-exangle);


  move_to_position(a1,a2);
}



//参数结构体
pid_handler Ppidx = //位置环(->角度环)xpid参数
{
  .P = 0,
  .I = 0,
  .D = 0,
  .I2 = 0,
  .out_limit = {-10000,10000},
  .inte_limit = {-5000,5000},
  .d_state=0,
  .err_limit = 100
};


static uint8_t Pfirstrun=1;
void position_pid_run()  
{
  if(Pfirstrun)            //判断是否要初始化pid结构体
  {
    pid_reset(&Ppidx);
    Pfirstrun =0;    
  }

exangle = pid_calculate(&Ppidx,nxc-exc);

}










int laser_ms_tick=0;
void right_scan()  //向右扫描
{
  static uint8_t i=1;
  if(i)
  {
  laser_control=1;
    //zhang_y_zero();
   //HAL_Delay(50);

  laser_enable(0);
    i=0;
  }
  step_motor2(1);
  delay_us(120);
  if((nxc1!=0&&nyc1!=0)&&(abs(nxc1-oexc)<50)&&(abs(nyc1-oeyc)<70))
  {
    HAL_Delay(200);
    control_state = 1;
    current_x=0;
    current_y=0;
    laser_ms_tick = HAL_GetTick();
    Tfirstrun = 1;
  }
}

void left_scan()  //向左扫描
{
  static uint8_t i=1;
  if(i)
  {
  laser_control=1;
    //zhang_y_zero();
   //HAL_Delay(50);

  laser_enable(0);
    i=0;
  }
  step_motor2(0);
  delay_us(120);           //120us生成一次脉冲0
  if((nxc1!=0&&nyc1!=0)&&(abs(nxc1-oexc)<50)&&(abs(nyc1-oeyc)<70))    //检测目标
  {
    HAL_Delay(200);
    control_state = 1;
    current_x=0;
    current_y=0;
    laser_ms_tick = HAL_GetTick();
    Tfirstrun = 1;
  }
}

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

float xan = 0;
float yan = 0;

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */


/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MPU_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */
  SCB_EnableICache();
  /* USER CODE END 1 */

  /* MPU Configuration--------------------------------------------------------*/
  MPU_Config();

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  MX_USART3_UART_Init();
  MX_UART8_Init();
  MX_I2C2_Init();
  MX_USART6_UART_Init();
  MX_UART4_Init();
  /* USER CODE BEGIN 2 */
  //motor_kalman_init();    
  //tms2209_init();   //tms2209初始化
  // WitInit(WIT_PROTOCOL_NORMAL, 0x50);  //陀螺仪初始化
  // WitSerialWriteRegister(JY901_SensorUartSend);  //注册陀螺仪发送函数
  // WitRegisterCallBack(JY901_SensorDataUpdata);   //注册接收回调函数
  // WitDelayMsRegister(HAL_Delay);    //注册ms延时函数
  HAL_UART_RegisterCallback(&huart6,HAL_UART_RX_COMPLETE_CB_ID,WIT_callback);
  HAL_UART_Receive_IT(&huart6,&rx_buffer2,1);

  HAL_UART_RegisterCallback(&huart1,HAL_UART_RX_COMPLETE_CB_ID,Maixcam_RX_callback);
  HAL_UART_Receive_IT(&huart1,&Maixcam_RX_data,1);

  HAL_UART_RegisterCallback(&huart4,HAL_UART_RX_COMPLETE_CB_ID,m0g3507_RX_callback);
  HAL_UART_Receive_IT(&huart4,&g3507_RX_data,1);


  zhang_motor_init();

  OLED_Init();
  OLED_FullyClear();
//init_tanh_table();

  //zhang_get_y_angle();
  HAL_Delay(100);
//zhang_y_zero();
//zhang_motor_closeloop_y();     //设置闭环模式
//HAL_Delay(100);
//zhang_motor_current(700);     //设置电流
// HAL_Delay(100);
// zhang_motor_256_microsteps();//设置256细分
// HAL_Delay(100);
    zhang_motor_enable_x(0);
  HAL_Delay(100);
  zhang_motor_enable_y(0);
  HAL_Delay(100);



laser_enable(0);          
  // laser_enable(0);

  
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    
   //draw_counter();
    //draw_circle();
    //draw_cos();
  // move_to_position(0,500);
  // move_to_position(0,0);
  // move_to_position(500,500);
  // move_to_position(0,500);
 //usart_printf("%d\n",enconter);


  switch (control_state)
  {
case 0:

  control_state = menu_state(); //菜单
    break;

    //  case 4:
    //  laser_control =0;
    // draw_circle_1();

  case 1:

    if((HAL_GetTick()-laser_ms_tick)>500)     //设置laser_ms_tick后,大于1s后自动开启激光
    {
      laser_enable(1);
    }
    if(nxc1!=0&&nyc1!=0)   //追踪控制
    {
      //draw_circle_1();
    track_pid_run();
    }
    else
    {
    pid_reset(&pidx);
    pid_reset(&pidy);
    }
    break;
  case 2:
    state_qu = 1;
    right_scan();  //向左扫描，找到目标后进入目标追踪
    break;
  case 3:
    state_qu = 1;
    left_scan();
    break;
  case 4:
    circle_state =1;
    control_state =1;
    break;
  case 5:
//    draw_counter();
    break;


  default:
    break;
  }





    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Supply configuration update enable
  */
  HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 5;
  RCC_OscInitStruct.PLL.PLLN = 192;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_2;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

 /* MPU Configuration */

void MPU_Config(void)
{
  MPU_Region_InitTypeDef MPU_InitStruct = {0};

  /* Disables the MPU */
  HAL_MPU_Disable();

  /** Initializes and configures the Region and the memory to be protected
  */
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER0;
  MPU_InitStruct.BaseAddress = 0x0;
  MPU_InitStruct.Size = MPU_REGION_SIZE_4GB;
  MPU_InitStruct.SubRegionDisable = 0x87;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
  MPU_InitStruct.AccessPermission = MPU_REGION_NO_ACCESS;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_SHAREABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);
  /* Enables the MPU */
  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);

}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
