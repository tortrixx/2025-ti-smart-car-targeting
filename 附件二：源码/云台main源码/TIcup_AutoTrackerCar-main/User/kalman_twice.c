#include "kalman_twice.h"

/* second-order kalman filter on stm32 */








void kalman_filter_init(kalman_filter_t *F, kalman_filter_init_t *I)
{
  mat_init(&F->xhat, 2, 1, I->xhat_data);
  mat_init(&F->xhatminus, 2, 1, I->xhatminus_data);
  mat_init(&F->z, 2, 1, I->z_data);

  mat_init(&F->A, 2, 2, I->A_data);
  mat_init(&F->H, 2, 2, I->H_data);

  mat_init(&F->AT, 2, 2, I->AT_data);
  mat_trans(&F->A, &F->AT);

  mat_init(&F->HT, 2, 2, I->HT_data);
  mat_trans(&F->H, &F->HT);

  mat_init(&F->Q, 2, 2, I->Q_data);
  mat_init(&F->R, 2, 2, I->R_data);
  mat_init(&F->P, 2, 2, I->P_data);
  mat_init(&F->Pminus, 2, 2, I->Pminus_data);
  mat_init(&F->K, 2, 2, I->K_data);
}


float *kalman_filter_calc(kalman_filter_t *F, float signal1, float signal2)
{
  float TEMP_data[4] = {0, 0, 0, 0};
  float TEMP_data21[2] = {0, 0};
  mat TEMP,TEMP21;

  mat_init(&TEMP,2,2,(float *)TEMP_data);
  mat_init(&TEMP21,2,1,(float *)TEMP_data21);

  F->z.pData[0] = signal1;
  F->z.pData[1] = signal2;

  //1. xhat'(k)= A xhat(k-1)
  mat_mult(&F->A, &F->xhat, &F->xhatminus);

  //2. P'(k) = A P(k-1) AT + Q
  mat_mult(&F->A, &F->P, &F->Pminus);
  mat_mult(&F->Pminus, &F->AT, &TEMP);
  mat_add(&TEMP, &F->Q, &F->Pminus);

  //3. K(k) = P'(k) HT / (H P'(k) HT + R)
  mat_mult(&F->H, &F->Pminus, &F->K);
  mat_mult(&F->K, &F->HT, &TEMP);
  mat_add(&TEMP, &F->R, &F->K);

  mat_inv(&F->K, &F->P);
  mat_mult(&F->Pminus, &F->HT, &TEMP);
  mat_mult(&TEMP, &F->P, &F->K);

  //4. xhat(k) = xhat'(k) + K(k) (z(k) - H xhat'(k))
  mat_mult(&F->H, &F->xhatminus, &TEMP21);
  mat_sub(&F->z, &TEMP21, &F->xhat);
  mat_mult(&F->K, &F->xhat, &TEMP21);
  mat_add(&F->xhatminus, &TEMP21, &F->xhat);

  //5. P(k) = (1-K(k)H)P'(k)
  mat_mult(&F->K, &F->H, &F->P);
  mat_sub(&F->Q, &F->P, &TEMP);
  mat_mult(&TEMP, &F->Pminus, &F->P);

  F->filtered_value[0] = F->xhat.pData[0];
  F->filtered_value[1] = F->xhat.pData[1];

  return F->filtered_value;
}

kalman_filter_init_t IX;
kalman_filter_init_t IY;

kalman_filter_t kalmanX;
kalman_filter_t kalmanY;



void motor_kalman_init()
{

// 状态转移矩阵 A (2x2)
IX.A_data[0] = 1.0f;  IX.A_data[1] = 0.05f;
IX.A_data[2] = 0.0f;  IX.A_data[3] = 1.0f;

// 测量矩阵 H (2x2) -- 只测量位置，所以第二行全0
IX.H_data[0] = 1.0f;  IX.H_data[1] = 0.0f;
IX.H_data[2] = 0.0f;  IX.H_data[3] = 0.0f;

// 过程噪声协方差 Q (2x2) - 可调，初始建议
IX.Q_data[0] = 0.3f;  IX.Q_data[1] = 0.0f;
IX.Q_data[2] = 0.0f;    IX.Q_data[3] = 2.0f;

// 测量噪声协方差 R (2x2) - 位置测量噪声
IX.R_data[0] = 0.03f;   IX.R_data[1] = 0.0f;
IX.R_data[2] = 0.0f;    IX.R_data[3] = 0.0f;

// 估计误差协方差矩阵 P (2x2) - 初始值
IX.P_data[0] = 1.0f;    IX.P_data[1] = 0.0f;
IX.P_data[2] = 0.0f;    IX.P_data[3] = 1.0f;

// 其他矩阵置0即可
for (int i=0; i<4; i++) {
    IX.Pminus_data[i] = 0.0f;
    IX.K_data[i] = 0.0f;
    IX.AT_data[i] = 0.0f;
    IX.HT_data[i] = 0.0f;
}

IX.xhat_data[0] = 0.0f; // 位置初值
IX.xhat_data[1] = 0.0f; // 速度初值
IX.xhatminus_data[0] = 0.0f;
IX.xhatminus_data[1] = 0.0f;

IX.z_data[0] = 0.0f;
IX.z_data[1] = 0.0f;






// 状态转移矩阵 A (2x2)
IY.A_data[0] = 1.0f;  IY.A_data[1] = 0.05f;
IY.A_data[2] = 0.0f;  IY.A_data[3] = 1.0f;

// 测量矩阵 H (2x2) -- 只测量位置，所以第二行全0
IY.H_data[0] = 1.0f;  IY.H_data[1] = 0.0f;
IY.H_data[2] = 0.0f;  IY.H_data[3] = 0.0f;

// 过程噪声协方差 Q (2x2) - 可调，初始建议
IY.Q_data[0] = 0.001f;  IY.Q_data[1] = 0.0f;
IY.Q_data[2] = 0.0f;    IY.Q_data[3] = 0.003f;

// 测量噪声协方差 R (2x2) - 位置测量噪声
IY.R_data[0] = 0.03f;   IY.R_data[1] = 0.0f;
IY.R_data[2] = 0.0f;    IY.R_data[3] = 0.0f;

// 估计误差协方差矩阵 P (2x2) - 初始值
IY.P_data[0] = 1.0f;    IY.P_data[1] = 0.0f;
IY.P_data[2] = 0.0f;    IY.P_data[3] = 1.0f;

// 其他矩阵置0即可
for (int i=0; i<4; i++) {
    IY.Pminus_data[i] = 0.0f;
    IY.K_data[i] = 0.0f;
    IY.AT_data[i] = 0.0f;
    IY.HT_data[i] = 0.0f;
}

IY.xhat_data[0] = 0.0f; // 位置初值
IY.xhat_data[1] = 0.0f; // 速度初值
IY.xhatminus_data[0] = 0.0f;
IY.xhatminus_data[1] = 0.0f;

IY.z_data[0] = 0.0f;
IY.z_data[1] = 0.0f;

kalman_filter_init(&kalmanX, &IX);
kalman_filter_init(&kalmanY, &IY);

}


float *kalman_calcu_x(float now_pos)
{
return kalman_filter_calc(&kalmanX, now_pos, 0);
}


float *kalman_calcu_y(float now_pos)
{
return kalman_filter_calc(&kalmanY, now_pos, 0);
}