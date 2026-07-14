#include "Kalman.h"

/*
 * 标量卡尔曼滤波的一次更新。先使用 Q 预测本次估计误差，再使用 R 计算测量的权重，
 * 最后修正 Output。该实现不检查空指针，因此调用前必须传入已初始化的结构体地址。
 */

float KalmanFilter(KFPType_Struct kfp, float input)
{
    //估算协方差方程：当前 估算协方差 = 上次更新 协方差 + 过程噪声协方差
    kfp->P = kfp->P + kfp->Q;
 
    //卡尔曼增益方程：当前 卡尔曼增益 = 当前 估算协方差 / （当前 估算协方差 + 测量噪声协方差）
    kfp->G = kfp->P / (kfp->P + kfp->R);
 
    //更新最优值方程：当前 最优值 = 当前 估算值 + 卡尔曼增益 * （当前 测量值 - 当前 估算值）
    kfp->Output = kfp->Output + kfp->G * (input - kfp->Output); //当前 估算值 = 上次 最优值
 
    //更新 协方差 = （1 - 卡尔曼增益） * 当前 估算协方差。
    kfp->P = (1 - kfp->G) * kfp->P;
 
     return kfp->Output;
}
