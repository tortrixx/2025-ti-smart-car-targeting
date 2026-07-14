#include "wit.h"

/*
 * WIT 惯导串口协议（普通模式）每帧 11 字节：0x55、数据类型、8 字节数据、校验和。
 * 数据在 UART6 的单字节接收中断中组帧；本文件只负责解析，控制补偿在 main.c 中使用
 * wit_data.yaw 和 wit_data.gyro[2]。不要在这里直接驱动电机。
 */
/* 加速度低通滤波器。facc 的更新目前被注释，保留此配置供恢复前馈时使用。 */
LowPassFilter acc ={
    .Tf= 0.05
};

uint8_t rx_buffer2;
uint8_t frame_buffer[256];
uint8_t rx_wit_cnt;
int data_index = 0, data_length = 0;


int flag_ins;     // 每成功解析一帧后置 1，供其他模块判断惯导已有有效数据
float position[3];
float dt=0.01; // 预留采样周期（秒）；当前解析流程未使用


float facc =0;

SensorData wit_data; // 解析后的物理量：角度单位为度，角速度单位为度/秒，加速度单位为 g

UART_State rx_state = STATE_WAIT_HEADER1;


// 全局变量定义
#define WIT_FRAME_SIZE 11
static WitState wit_state = WIT_STATE_WAIT_HEADER;



// 数据处理函数声明
void WitNormalDataProcess(uint8_t *data, uint16_t size);

/*
 * UART6 单字节接收完成回调。状态机仅接受以 0x55 开始的完整 11 字节帧；接收完成
 * 后一定要重新启动 HAL_UART_Receive_IT()，否则后续帧不会再触发中断。
 */
void WIT_callback(UART_HandleTypeDef *huart)
{
	huart = huart;

	
//	if(huart -> Instance == USART3)
//	{
//		printf("%c",arr3);
//		HAL_UART_Receive_IT(&huart3,&arr3,1);
//		
//	}
	
	
	
	

						

			
        switch(wit_state)
        {
            case WIT_STATE_WAIT_HEADER:
                // 检查是否为帧头0x55
                if(rx_buffer2 == 0x55) {
                    frame_buffer[0] = rx_buffer2;
                    rx_wit_cnt = 1;
                    wit_state = WIT_STATE_RECEIVING;
                }
                break;
                
            case WIT_STATE_RECEIVING:
                // 存储接收到的数据
                if(rx_wit_cnt < WIT_FRAME_SIZE) {
                    frame_buffer[rx_wit_cnt] = rx_buffer2;
                    rx_wit_cnt++;
                    
                    // 检查是否接收完整帧
                    if(rx_wit_cnt == WIT_FRAME_SIZE) {
                        wit_state = WIT_STATE_COMPLETE;
                    }
                } else {
                    // 缓冲区溢出，重置状态机
                    wit_state = WIT_STATE_WAIT_HEADER;
                    rx_wit_cnt = 0;
                }
                break;
                
            case WIT_STATE_COMPLETE:
                // 处理完整帧数据
                WitNormalDataProcess(frame_buffer, WIT_FRAME_SIZE);
						
                
                // 重置状态机
                wit_state = WIT_STATE_WAIT_HEADER;
                rx_wit_cnt = 0;
						
						
						    //惯导标志位
						    flag_ins = 1;
						
						
                
                // 存储当前字节作为下一帧的起始
                if(rx_buffer2 == 0x55) {
                    frame_buffer[0] = rx_buffer2;
                    rx_wit_cnt = 1;
                    wit_state = WIT_STATE_RECEIVING;
                }
                break;
        }
        
        // 重新启动接收中断
        HAL_UART_Receive_IT(&huart6,&rx_buffer2,1);
    }

/* 记录 yaw 穿越 +180/-180 度的次数，将传感器的 [-180, 180] 度展开成连续角度。 */
static float last_yaw = 0;  // 上一次的偏航角
static int32_t yaw_rounds = 0;  // 圈数计数
static uint8_t first_yaw = 1;   // 首次读取标志
/*
 * 校验并解析一帧 WIT 普通模式数据。data 必须至少有 11 字节，且 data[10] 是前十个
 * 字节的 8 位累加校验和。未通过校验的帧会被直接丢弃，避免将串口噪声送入控制环。
 */
void WitNormalDataProcess(uint8_t *data, uint16_t size)
{
    size = size;
    // 校验和验证 (前10字节累加和)
    uint8_t calc_sum = 0;
    for(int i = 0; i < 10; i++) {
        calc_sum += data[i];
    }
    
    // 校验失败处理
    if(calc_sum != data[10]) {
        // 这里可以添加错误处理逻辑
        return;
    }
    
    // 提取数据类型标识
    uint8_t data_type = data[1];
    
    // 解析4组16位数据 (小端模式)
    uint16_t sensor_data[4];
    sensor_data[0] = ((uint16_t)data[3] << 8) | data[2];
    sensor_data[1] = ((uint16_t)data[5] << 8) | data[4];
    sensor_data[2] = ((uint16_t)data[7] << 8) | data[6];
    sensor_data[3] = ((uint16_t)data[9] << 8) | data[8];
    
    // 根据数据类型处理传感器数据
    switch(data_type) {
        case 0x51: // 加速度：量程按 ±16 g 换算
    				wit_data.accel[0] = (int16_t)sensor_data[0] / 32768.0f * 16.0f;
            wit_data.accel[1] = (int16_t)sensor_data[1] / 32768.0f * 16.0f;
            wit_data.accel[2] = (int16_t)sensor_data[2] / 32768.0f * 16.0f;
				//facc = LowPassFilterRun(&acc,wit_data.accel[1]);
            break;
            
        case 0x52: // 角速度：量程按 ±2000 °/s 换算
            wit_data.gyro[0] = (int16_t)sensor_data[0] / 32768.0f * 2000.0f;
            wit_data.gyro[1] = (int16_t)sensor_data[1] / 32768.0f * 2000.0f;
            wit_data.gyro[2] = (int16_t)sensor_data[2] / 32768.0f * 2000.0f;
            break;
            
        case 0x53: // 欧拉角：roll、pitch、yaw 的原始量程均为 ±180 度
            wit_data.roll = (int16_t)sensor_data[0] / 32768.0f * 180.0f;
            wit_data.pitch = (int16_t)sensor_data[1] / 32768.0f * 180.0f;
            
            // 临时保存当前读取到的yaw角度
            float current_yaw = (int16_t)sensor_data[2] / 32768.0f * 180.0f;
            
            if(first_yaw) {
                last_yaw = current_yaw;
                first_yaw = 0;
            } else {
                // 计算角度差
                float delta_yaw = current_yaw - last_yaw;
                
                // 处理跨越±180°的情况
                if(delta_yaw > 180) {
                    yaw_rounds--;
                } else if(delta_yaw < -180) {
                    yaw_rounds++;
                }
                
                last_yaw = current_yaw;
            }
            
            // 计算累积的总角度
            wit_data.yaw = current_yaw + yaw_rounds * 360.0f;
            break;
            
        case 0x54: // 磁场
            // data[0:2] = X/Y/Z (单位: 0.1uT)
            // 处理磁场数据...
            break;
            
        default:
            // 未知数据类型处理
            break;
    }
}

//// 初始化函数 - 在main中调用
//void UART_WIT_Init(void)
//{
//    // 启动第一次接收
//    HAL_UART_Receive_IT(&huart2, &rx_buffer2, 1);
//    
//    // 初始化状态机
//    wit_state = WIT_STATE_WAIT_HEADER;
//    rx_wit_cnt = 0;
//}



