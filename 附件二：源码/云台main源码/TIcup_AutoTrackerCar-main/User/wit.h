#ifndef __WIT_H__
#define __WIT_H__

#include "main.h"
#include "usart.h"

// ״̬��״̬����
typedef enum {
    WIT_STATE_WAIT_HEADER,   // �ȴ�֡ͷ״̬
    WIT_STATE_RECEIVING,      // ��������״̬
    WIT_STATE_COMPLETE        // ��ɽ���״�?
} WitState;

// ���������ݽṹ��
typedef struct {
    float roll;      // ��ת�� (��)
    float pitch;     // ������ (��)
    float yaw;       // ƫ���� (��)
    float gyro[3];   // ���ٶ� [x, y, z] (��/��)
    float accel[3];  // ���ٶ� [x, y, z] (g)
} SensorData;

extern SensorData wit_data;

typedef enum{
	STATE_WAIT_HEADER1,
	STATE_WAIT_HEADER2,
	STATE_RECEIVE_DATA,
	STATE_WAIT_FOOTER1,
	STATE_WAIT_FOOTER2
} UART_State;

extern UART_State rx_state;
void WIT_callback(UART_HandleTypeDef *huart);



void WitNormalDataProcess(uint8_t *data, uint16_t size);
void WIT_callback(UART_HandleTypeDef *huart);

extern uint8_t rx_buffer2;






#endif 
