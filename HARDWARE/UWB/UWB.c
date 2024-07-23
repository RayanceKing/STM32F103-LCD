#include "uwb.h"
#include "delay.h"
#include "LoRa.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// 定义基站坐标
#define BASE1_X 0
#define BASE1_Y 0
#define BASE2_X 320
#define BASE2_Y 0
#define BASE3_X 320
#define BASE3_Y 320

// 存储UWB模块的坐标
uint16_t uwb_x = 0;
uint16_t uwb_y = 0;

// 初始化UWB模块
void UWB_Init(void) {
    // 初始化USART1用于UWB通信
    USART1_Init();

    // 如果需要的话，可以添加额外的初始化代码
}

// 通过USART发送数据
void UWB_SendData(uint8_t data) {
    while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
    USART_SendData(USART1, data);
}

// 通过USART发送字符串
void USART_SendString(char* str) {
    while (*str) {
        UWB_SendData(*str++);
    }
}

// 通过USART接收数据
uint8_t UWB_ReceiveData(void) {
    while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);
    return USART_ReceiveData(USART1);
}


// 获取UWB数据并处理
void UWB_GetCoordinates(void) {
    char buffer[50];
    uint8_t i = 0;
    uint8_t received_byte;

    // 接收数据，直到收到换行符或缓冲区满
    do {
        received_byte = UWB_ReceiveData();
        buffer[i++] = received_byte;
        delay_ms(10); // 添加10ms延时，防止数据接收过快
    } while (received_byte != '\n' && i < sizeof(buffer) - 1);

    // 确保缓冲区以空字符结尾
    buffer[i] = '\0';

    // 处理接收到的UWB坐标并发送到LoRa
    process_and_send_uwb_coordinates(buffer);
}
