#include "Zhangdatou.h"
#include "usart.h"

#define uart_motorx huart2
#define uart_motory huart3
uint8_t RXdata_x=0;
uint8_t RXdata_y=0;

uint8_t motor_x_recall=0;
uint8_t motor_y_recall=0;


uint8_t y_uart_stage=0;
uint8_t yrxdata[10];
uint16_t Mencounder=0;

int i =0;


void motorx_callback()
{

    motor_x_recall=1;
    HAL_UART_Receive_IT(&uart_motorx,&RXdata_x,1);
}
void motory_callback()
{
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



void zhang_motor_init()
{
    HAL_UART_RegisterCallback(&uart_motorx,HAL_UART_RX_COMPLETE_CB_ID,motorx_callback);
    HAL_UART_RegisterCallback(&uart_motory,HAL_UART_RX_COMPLETE_CB_ID,motory_callback);
    HAL_UART_Receive_IT(&uart_motorx,&RXdata_x,1);
    HAL_UART_Receive_IT(&uart_motory,&RXdata_y,1);
    // HAL_UARTEx_ReceiveToIdle_IT(&uart_motorx,RXdata_x,20);
    // HAL_UARTEx_ReceiveToIdle_IT(&uart_motory,RXdata_y,20);
}


void zhang_motor_enable_x(uint8_t state)
{
    uint8_t data[20] = {0x01,0xF3,0xAB,0x01,0x00,0x6B};
    data[3] = state;
    HAL_UART_Transmit(&uart_motorx,data,6,0xffff);
    // while (motor_x_recall==0);

    // motor_x_recall =0;
}

void zhang_motor_enable_y(uint8_t state)
{
    uint8_t data[20] = {0x01,0xF3,0xAB,0x01,0x00,0x6B};
    data[3] = state;
    HAL_UART_Transmit(&uart_motory,data,6,0xffff);
    // while (motor_y_recall==0);
    // motor_y_recall =0;
}

void zhang_motor_openloop_x()
{
uint8_t data[20] = {0x01,0x46,0x69,0x01,0x01,0x6B};

    HAL_UART_Transmit(&uart_motorx,data,6,0xffff);   
    //HAL_UART_Transmit(&uart_motory,data,6,0xffff);   
}
void zhang_motor_closeloop_x()
{
uint8_t data[20] = {0x01,0x46,0x69,0x01,0x02,0x6B};

    HAL_UART_Transmit(&uart_motorx,data,6,0xffff);   
    //HAL_UART_Transmit(&uart_motory,data,6,0xffff);   
}

void zhang_motor_openloop_y()
{
uint8_t data[20] = {0x01,0x46,0x69,0x01,0x01,0x6B};

    //HAL_UART_Transmit(&uart_motorx,data,6,0xffff);   
    HAL_UART_Transmit(&uart_motory,data,6,0xffff);   
}
void zhang_motor_closeloop_y()
{
uint8_t data[20] = {0x01,0x46,0x69,0x01,0x02,0x6B};

    //HAL_UART_Transmit(&uart_motorx,data,6,0xffff);   
    HAL_UART_Transmit(&uart_motory,data,6,0xffff);   
}


void zhang_motor_current(uint16_t current)
{
uint8_t data[20] = {0x01,0x44,0x33,0x00,0x03,0xE8,0x6B};
    data[4] = (current>>8)&0xff;
    data[5] = current&0xff;

    HAL_UART_Transmit(&uart_motorx,data,7,0xffff);   
    HAL_UART_Transmit(&uart_motory,data,7,0xffff);   
}
void zhang_motor_256_microsteps()
{
uint8_t data[20] = {0x01,0x84,0x8A,0x01,0x00,0x6B};



    HAL_UART_Transmit(&uart_motorx,data,6,0xffff);   
    HAL_UART_Transmit(&uart_motory,data,6,0xffff);  

}


void zhang_y_zero()
{
uint8_t data[20] = {0x01,0x9A,0x01,0x00,0x6B};

    HAL_UART_Transmit(&uart_motory,data,5,0xffff);  
    
}

void zhang_set_zero()
{
uint8_t data[20] = {0x01,0x93,0x88,0x01,0x6B};

    HAL_UART_Transmit(&uart_motory,data,5,0xffff);  

}


void zhang_get_y_angle()
{
    uint8_t data[20] = {0x01,0x31,0x6B};

    HAL_UART_Transmit(&uart_motory,data,3,0xffff);  
}