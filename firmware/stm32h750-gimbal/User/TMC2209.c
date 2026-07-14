#include "TMC2209.h"
#include "usart.h"



void swuart_calcCRC(uint8_t* datagram, uint8_t datagramLength)
{
 int i,j;
 uint8_t* crc = datagram + (datagramLength-1); // CRC located in last byte of message
 uint8_t currentByte;
 *crc = 0;
 for (i=0; i<(datagramLength-1); i++) { // Execute for all bytes of a message
 currentByte = datagram[i]; // Retrieve a byte to be sent from Array
 for (j=0; j<8; j++) {
 if ((*crc >> 7) ^ (currentByte&0x01)) // update CRC based result of XOR operation
 {
 *crc = (*crc << 1) ^ 0x07;
 }
 else
 {
 *crc = (*crc << 1);
 }
 currentByte = currentByte >> 1;
 } // for CRC bit
 } // for message byte
}


void write_reg(uint8_t device,uint8_t addr,uint8_t reg,uint32_t data)
{
    uint8_t TX_data[8];
    TX_data[0] = 0x05;
    TX_data[1] = addr;
    TX_data[2] = reg|0x80;
    TX_data[3] = data>>24;
    TX_data[4] = data>>16;
    TX_data[5] = data>>8;
    TX_data[6] = data&0xFF;
    swuart_calcCRC(TX_data,8);
    if(device)
    {
HAL_UART_Transmit(&huart2,TX_data,8,0xffff);
    }
    else
    {
        HAL_UART_Transmit(&huart3,TX_data,8,0xffff);
    }
}

void read_reg(uint8_t device,uint8_t addr,uint8_t reg)
{
    uint8_t RX_data[8];
    uint8_t TX_data[4];
    TX_data[0] = 0x05;
    TX_data[1] = addr;
    TX_data[2] = reg;
    swuart_calcCRC(TX_data,4);
    if(device)
    {
    HAL_UART_Transmit(&huart2,TX_data,4,0xffff);
    HAL_UART_Receive(&huart2,RX_data,8,0xffff);
    }
    else
    {
     HAL_UART_Transmit(&huart3,TX_data,4,0xffff);
    HAL_UART_Receive(&huart3,RX_data,8,0xffff);       
    }
    
}


void tms2209_init()
{
    uint8_t data[8]={0};
    data[0] = 0x81;
    data[1] = 0x00;
    write_reg(1,0x00,GCONF,*((uint32_t*)&data));
read_reg(1,0x00,GCONF);
    write_reg(0,0x00,GCONF,*((uint32_t*)&data));
//read_reg(0,0x00,GCONF);
    data[3] = 0x10;
    data[2] = 0x01;
    data[1] = 0x00;
    data[0] = 0x53;

    write_reg(1,0x00,CHOPCONF,*((uint32_t*)&data));
    read_reg(1,0x00,CHOPCONF);
    write_reg(0,0x00,CHOPCONF,*((uint32_t*)&data));
    //read_reg(0,0x00,CHOPCONF);
}