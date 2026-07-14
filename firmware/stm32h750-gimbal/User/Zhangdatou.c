#include "Zhangdatou.h"
#include "usart.h"

#define uart_motorx huart2
#define uart_motory huart3
/* 驱动器的回应按字节接收；命令发送均为阻塞式。 */
uint8_t RXdata_x=0;
uint8_t RXdata_y=0;

uint8_t motor_x_recall=0;
uint8_t motor_y_recall=0;


/* Y 轴回应状态机：检测到 0x01, 0x31 后再采集随后的两个数据字节。 */
uint8_t y_uart_stage=0;
uint8_t yrxdata[10];
uint16_t Mencounder=0;

int i =0;


/* X 轴收到任意应答即置位，并立即挂起下一次单字节接收。 */
void motorx_callback(UART_HandleTypeDef *huart)
{
    (void)huart;

    motor_x_recall=1;
    HAL_UART_Receive_IT(&uart_motorx,&RXdata_x,1);
}
/*
 * Y 轴的 0x01 0x31 开头的回应中，随后两个字节按小端组合为 Mencounder。
 * 回调中不做等待；解析完成后必须重新调用 HAL_UART_Receive_IT() 保持接收。
 */
void motory_callback(UART_HandleTypeDef *huart)
{
    (void)huart;
    if(RXdata_y ==0x01&&y_uart_stage==0)
    {
        y_uart_stage=1;
    }

    if(RXdata_y ==0x31&&y_uart_stage==1)
    {
        y_uart_stage=2;
    }
    // else if(y_uart_stage==1)
    // {
    //     y_uart_stage=0;
    // }

    if(y_uart_stage==2)
    {
        yrxdata[i++] = RXdata_y;
        if(i==2)
        {
            Mencounder = *(uint16_t *)yrxdata;
            i=0;
            y_uart_stage=0;
        }
    }


    motor_y_recall=1;
    HAL_UART_Receive_IT(&uart_motory,&RXdata_y,1);
}



/* 为 UART2/UART3 注册接收完成回调，并启动首次接收。 */
void zhang_motor_init()
{
    HAL_UART_RegisterCallback(&uart_motorx,HAL_UART_RX_COMPLETE_CB_ID,motorx_callback);
    HAL_UART_RegisterCallback(&uart_motory,HAL_UART_RX_COMPLETE_CB_ID,motory_callback);
    HAL_UART_Receive_IT(&uart_motorx,&RXdata_x,1);
    HAL_UART_Receive_IT(&uart_motory,&RXdata_y,1);
    // HAL_UARTEx_ReceiveToIdle_IT(&uart_motorx,RXdata_x,20);
    // HAL_UARTEx_ReceiveToIdle_IT(&uart_motory,RXdata_y,20);
}


/* 发送 X 轴使能命令：第 4 字节为状态，0 为失能，非 0 为使能。 */
void zhang_motor_enable_x(uint8_t state)
{
    uint8_t data[20] = {0x01,0xF3,0xAB,0x01,0x00,0x6B};
    data[3] = state;
    HAL_UART_Transmit(&uart_motorx,data,6,0xffff);
    // while (motor_x_recall==0);

    // motor_x_recall =0;
}

/* 发送 Y 轴使能命令：第 4 字节为状态，0 为失能，非 0 为使能。 */
void zhang_motor_enable_y(uint8_t state)
{
    uint8_t data[20] = {0x01,0xF3,0xAB,0x01,0x00,0x6B};
    data[3] = state;
    HAL_UART_Transmit(&uart_motory,data,6,0xffff);
    // while (motor_y_recall==0);
    // motor_y_recall =0;
}

/* X 轴开环：驱动器不再以自身位置反馈修正运动。 */
void zhang_motor_openloop_x()
{
uint8_t data[20] = {0x01,0x46,0x69,0x01,0x01,0x6B};

    HAL_UART_Transmit(&uart_motorx,data,6,0xffff);   
    //HAL_UART_Transmit(&uart_motory,data,6,0xffff);   
}
/* X 轴闭环：需确认驱动器反馈与机构零点已经正确配置。 */
void zhang_motor_closeloop_x()
{
uint8_t data[20] = {0x01,0x46,0x69,0x01,0x02,0x6B};

    HAL_UART_Transmit(&uart_motorx,data,6,0xffff);   
    //HAL_UART_Transmit(&uart_motory,data,6,0xffff);   
}

/* Y 轴开环。 */
void zhang_motor_openloop_y()
{
uint8_t data[20] = {0x01,0x46,0x69,0x01,0x01,0x6B};

    //HAL_UART_Transmit(&uart_motorx,data,6,0xffff);   
    HAL_UART_Transmit(&uart_motory,data,6,0xffff);   
}
/* Y 轴闭环。 */
void zhang_motor_closeloop_y()
{
uint8_t data[20] = {0x01,0x46,0x69,0x01,0x02,0x6B};

    //HAL_UART_Transmit(&uart_motorx,data,6,0xffff);   
    HAL_UART_Transmit(&uart_motory,data,6,0xffff);   
}


/*
 * 同时设置两轴的电流设定值。协议将 16 位数拆为高字节、低字节发送；不要根据
 * 变量类型猜测单位，应以驱动器说明书和实测温升确定安全值。
 */
void zhang_motor_current(uint16_t current)
{
uint8_t data[20] = {0x01,0x44,0x33,0x00,0x03,0xE8,0x6B};
    data[4] = (current>>8)&0xff;
    data[5] = current&0xff;

    HAL_UART_Transmit(&uart_motorx,data,7,0xffff);   
    HAL_UART_Transmit(&uart_motory,data,7,0xffff);   
}
/* 向两轴发送固定的 256 微步配置。 */
void zhang_motor_256_microsteps()
{
uint8_t data[20] = {0x01,0x84,0x8A,0x01,0x00,0x6B};



    HAL_UART_Transmit(&uart_motorx,data,6,0xffff);   
    HAL_UART_Transmit(&uart_motory,data,6,0xffff);  

}


/* 让 Y 轴执行回零/校零命令；调用前必须保证机械限位和工作区安全。 */
void zhang_y_zero()
{
uint8_t data[20] = {0x01,0x9A,0x01,0x00,0x6B};

    HAL_UART_Transmit(&uart_motory,data,5,0xffff);  
    
}

/* 将当前位置写为零点。该命令只发送给 Y 轴。 */
void zhang_set_zero()
{
uint8_t data[20] = {0x01,0x93,0x88,0x01,0x6B};

    HAL_UART_Transmit(&uart_motory,data,5,0xffff);  

}


/* 请求 Y 轴角度，异步回应由 motory_callback() 解析。 */
void zhang_get_y_angle()
{
    uint8_t data[20] = {0x01,0x31,0x6B};

    HAL_UART_Transmit(&uart_motory,data,3,0xffff);  
}
