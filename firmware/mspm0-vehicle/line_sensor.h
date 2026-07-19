#ifndef LINE_SENSOR_H_
#define LINE_SENSOR_H_

#include <stdbool.h>
#include <stdint.h>

/*
 * line_sensor.h - 8·GPIO���������ִ�����ӿ�
 * line_sensor.h - 8-channel GPIO digital grayscale sensor interface
 *
 * Ӳ������ / Hardware:
 *   X1=PA15, X2=PA17, X3=PA22, X4=PA24
 *   X5=PA25, X6=PA26, X7=PA27, X8=PB20
 *
 * ������ / Sensor type:
 *   ���ִ����������ֵ�Ƚ����������0=�ڣ����棩��1=�ף��ر���
 *   Digital sensor with comparator. Output: 0=black(absorb), 1=white(reflect)
 *   У׼��ȡ��: value[i]=1��ʾ�ڵ�(�й���)
 *   After calibration: value[i]=1 means black line detected
 */

#define LINE_SENSOR_COUNT          (8U)

/* ���������ݽṹ / Sensor data structure */
typedef struct {
    uint8_t raw;                       /* ԭʼ8λֵ / raw 8-bit value */
    uint8_t value[LINE_SENSOR_COUNT];  /* У׼��ֵ: value[0]=X1(����), 1=�ڣ�0=�� */
                                        /* Calibrated: value[0]=X1(leftmost), 1=black, 0=white */
} LineSensor_Data;

/* 传感器初始化 (16次采样校准) / Sensor init (16-sample calibration) */
void LineSensor_Init(void);

/* 校准是否成功 / Whether calibration succeeded */
bool LineSensor_IsCalibrated(void);

/* 获取校准白值 (调试用) / Get white calibration pattern (for debugging) */
uint8_t LineSensor_GetWhitePattern(void);

/* 读取原始值 (3次读取投票去抖动) / Read raw (3-sample majority vote debounce) */
bool LineSensor_ReadRaw(uint8_t *raw);

/* 读取校准后传感器数据 / Read calibrated sensor data */
bool LineSensor_Read(LineSensor_Data *data);

#endif /* LINE_SENSOR_H_ */
