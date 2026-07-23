#ifndef APP_CONFIG_H_
#define APP_CONFIG_H_

#include <stdint.h>

/*
 * app_config.h - 循迹小车可调参数（参考已验证项目校准）
 * app_config.h - Line-following car tunable parameters (calibrated)
 *
 * 2025电赛基础部分: 正方形环形黑线循迹 / 2025 basic task: square loop
 *
 * PWM范围 / PWM range
 * PWM周期计数器最大为1000 (TIMG 100kHz / 1000 = 100Hz)
 */
#define CAR_PWM_MAX                (1000)

/*
 * 基础速度 / Base speed (PWM duty 0~CAR_PWM_MAX)
 * 参考已验证值: 148
 */
#define CAR_BASE_SPEED             (148)

/* 转弯最小速度 (防停转) / Minimum turn speed (prevent stall) */
#define CAR_MIN_TURN_SPEED         (88)

/* 转弯时内圈最小速度 / Minimum inner wheel speed during turn */
#define CAR_MIN_INNER_SPEED        (24)

/*
 * 位置PD参数 / Position PD parameters
 *
 * error = Σ(weight[i] × is_black[i]) / black_count
 * error_range ≈ -3500 ~ +3500
 *
 * turn = (error * KP + derivative * KD) / TURN_SCALE
 */
#define CAR_POSITION_KP            (8)
#define CAR_POSITION_KD            (7)
#define CAR_TURN_SCALE             (100)

/* D term low-pass filter: 1/4 of the new derivative per 10ms tick. */
#define CAR_DERIVATIVE_FILTER_DIV  (4)

/* 转向最大量 / Turn delta limit (per cycle) */
#define CAR_MAX_TURN_DELTA         (190)

/* 转向步进限制 (防突变) / Turn step limit (anti-jerk) */
#define CAR_TURN_STEP_LIMIT        (160)

/*
 * 自适应速度 / Adaptive speed
 * speed = BASE_SPEED - |error| / SLOWDOWN_DIV
 * 误差越大速度越低 → 弯道自动减速
 */
#define CAR_SPEED_SLOWDOWN_DIV     (28)

/* 多线检测时额外减速 / Slowdown when multi-line detected */
#define CAR_MULTI_LINE_SLOWDOWN    (6)

/* Slow down further when steering demand is high. */
#define CAR_TURN_SLOWDOWN_DIV      (6)

/* Per-10ms target-duty slew limits. */
#define CAR_SPEED_SLEW_UP           (12)
#define CAR_SPEED_SLEW_DOWN         (30)

/* 转向死区 (小于此值视为error≈0) / Turn deadband */
#define CAR_TURN_DEADBAND          (320)

/* 小误差转弯减半 (error < 此值时turn减半) / Fine-tune range */
#define CAR_FINE_TURN_ERROR        (900)

/*
 * 丢线恢复 / Lost-line recovery
 *
 * 正方形赛道直角弯处短暂丢线 → 保持转向惯性直行穿越 →
 * 重新检测到线 → 恢复巡线
 */
#define CAR_BRIDGE_SPEED              (138)
#define CAR_BRIDGE_ENABLE             (1)
#define CAR_BRIDGE_MIN_STRAIGHT_TICKS (20)
#define CAR_LINE_REACQUIRE_TICKS      (3)
#define CAR_BRIDGE_MAX_TICKS          (200)

#define CAR_SOLID_BLACK_COUNT         (7)

/*
 * 电机转向取反 (0=正常, 1=取反)
 * Motor direction invert
 */
#define MOTOR_LEFT_INVERT          (0)
#define MOTOR_RIGHT_INVERT         (1)

#endif /* APP_CONFIG_H_ */
