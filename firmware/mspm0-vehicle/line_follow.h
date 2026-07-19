#ifndef LINE_FOLLOW_H_
#define LINE_FOLLOW_H_

#include <stdbool.h>
#include <stdint.h>

/*
 * line_follow.h - 循迹状态机接口
 * line_follow.h - Line-following state machine interface
 *
 * 2025电赛基础部分: 正方形环形黑线循迹
 * 2025 basic task: square loop black line tracking
 */

void LineFollower_Reset(void);
bool LineFollower_Task(void);

/*
 * 调试变量 (供CCS debugger观察)
 * Debug variables (visible in CCS debugger watch window)
 *
 * 在CCS Expressions窗口中添加以下变量名即可实时观察
 * Add these variable names to CCS Expressions window for live monitoring
 */
extern int16_t g_debug_error;         /* 位置误差 / position error (-3500~+3500) */
extern uint8_t g_debug_black_count;   /* 检测到黑线的传感器数量 / sensors seeing black */
extern uint8_t g_debug_sensor_bits;   /* 原始传感器位值 / raw sensor bit pattern */
extern uint8_t g_debug_follower_mode; /* 状态机模式 (0=WAIT,1=TRACK,2=BRIDGE, 0xFF=校准失败) */
extern int16_t g_debug_left_speed;    /* 左轮目标速度 / left wheel target speed */
extern int16_t g_debug_right_speed;   /* 右轮目标速度 / right wheel target speed */
extern int16_t g_debug_turn;          /* 转向量 / turn delta */

#endif /* LINE_FOLLOW_H_ */
