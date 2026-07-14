#ifndef __KALMAN_TWICE_H
#define __KALMAN_TWICE_H
#include "arm_math.h"
#include "stm32h7xx_hal.h"

#define mat         arm_matrix_instance_f32 
#define mat_init    arm_mat_init_f32
#define mat_add     arm_mat_add_f32
#define mat_sub     arm_mat_sub_f32
#define mat_mult    arm_mat_mult_f32
#define mat_trans   arm_mat_trans_f32
#define mat_inv     arm_mat_inverse_f32
typedef struct
{
  float raw_value;          /* 预留的原始值字段，当前未在计算中使用 */
  float filtered_value[2];  /* 输出状态 [位置, 速度] */
  mat xhat, xhatminus, z, A, H, AT, HT, Q, R, P, Pminus, K;
} kalman_filter_t;

/*
 * 二阶（位置 + 速度）卡尔曼滤波器的实际存储区。CMSIS 矩阵对象只保存指针和尺寸，
 * 因此此结构体必须在滤波器整个生命周期内有效，不能用函数内局部变量替代。
 */
typedef struct
{
  float raw_value;
  float filtered_value[2];
  float xhat_data[2], xhatminus_data[2], z_data[2],Pminus_data[4], K_data[4];
  float P_data[4];
  float AT_data[4], HT_data[4];
  float A_data[4];
  float H_data[4];
  float Q_data[4];
  float R_data[4];
} kalman_filter_init_t;

/* 把存储区绑定到 CMSIS-DSP 矩阵对象，并由 A/H 生成其转置矩阵。 */
void kalman_filter_init(kalman_filter_t *F, kalman_filter_init_t *I);
/* 输入本周期的两个测量量，返回 [位置估计, 速度估计]。 */
float *kalman_filter_calc(kalman_filter_t *F, float signal1, float signal2);

/* 初始化 X/Y 两套滤波器。若要启用 kalman_calcu_x/y，必须先调用一次。 */
void motor_kalman_init();
float *kalman_calcu_x(float now_pos);
float *kalman_calcu_y(float now_pos);










#endif
