#include "wit.h"



LowPassFilter acc ={
    .Tf= 0.05
};

uint8_t rx_buffer2;
uint8_t frame_buffer[256];
uint8_t rx_wit_cnt;
int data_index = 0, data_length = 0;


int flag_ins;     //惯导标志位
float position[3];
float dt=0.01; // 假设100Hz采样率


float facc =0;

SensorData wit_data;

UART_State rx_state = STATE_WAIT_HEADER1;


// 全局变量定义
#define WIT_FRAME_SIZE 11
static WitState wit_state = WIT_STATE_WAIT_HEADER;



// 数据处理函数声明
void WitNormalDataProcess(uint8_t *data, uint16_t size);

// UART接收完成中断回调
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

// 添加全局变量用于跟踪圈数和上一次的角度值
static float last_yaw = 0;  // 上一次的偏航角
static int32_t yaw_rounds = 0;  // 圈数计数
static uint8_t first_yaw = 1;   // 首次读取标志
// WIT普通模式数据处理函数
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
        case 0x51: // 加速度
            // data[0:2] = X/Y/Z (单位: mg)
            // 处理加速度数据...
            // 转换为g (除以1000)
    				wit_data.accel[0] = (int16_t)sensor_data[0] / 32768.0f * 16.0f;
            wit_data.accel[1] = (int16_t)sensor_data[1] / 32768.0f * 16.0f;
            wit_data.accel[2] = (int16_t)sensor_data[2] / 32768.0f * 16.0f;
				//facc = LowPassFilterRun(&acc,wit_data.accel[1]);
            break;
            
        case 0x52: // 角速度
            // data[0:2] = X/Y/Z (单位: 0.1°/s)
            // 处理角速度数据...
            // 转换为°/s (除以10)
            wit_data.gyro[0] = (int16_t)sensor_data[0] / 32768.0f * 2000.0f;
            wit_data.gyro[1] = (int16_t)sensor_data[1] / 32768.0f * 2000.0f;
            wit_data.gyro[2] = (int16_t)sensor_data[2] / 32768.0f * 2000.0f;
            break;
            
        case 0x53: // 欧拉角
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




