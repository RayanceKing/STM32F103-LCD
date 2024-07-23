#include "LoRa.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h> // 添加标准库头文件

#define LORA_TX_BUFFER_SIZE  256
#define LORA_RX_BUFFER_SIZE  256

// 静态发送和接收缓冲区
static uint8_t txBuffer[LORA_TX_BUFFER_SIZE];
static uint8_t rxBuffer[LORA_RX_BUFFER_SIZE];

// 初始化 USART1
void USART1_Init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    // 打开时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);

    // 配置 USART1 Tx (PA.09)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // 配置 USART1 Rx (PA.10)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // 配置 USART1 初始化参数
    USART_InitTypeDef USART_InitStructure;
    USART_InitStructure.USART_BaudRate = 9600;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART1, &USART_InitStructure);

    // 使能 USART1
    USART_Cmd(USART1, ENABLE);

    // 使能接收中断
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    NVIC_EnableIRQ(USART1_IRQn);
}

// 初始化 LoRa 模块
void LoRa_Init(LoRa_ConfigTypeDef *config) {
    // 初始化 USART1
    USART1_Init();

    // 配置 LoRa 模块参数，例如频率、扩频因子、带宽等
    // 示例：
    //LoRa_SetRxMode();
}

// 设置 LoRa 模块为发送模式
void LoRa_SetTxMode(void) {
    //LoRa_SendPacket(1, 0, 0); // 示例传递参数，具体参数根据需求设置
}

// 设置 LoRa 模块为接收模式
void LoRa_SetRxMode(void) {
    //LoRa_SendPacket(0, 0, 0); // 示例传递参数，具体参数根据需求设置
}

// 发送数据包
void LoRa_SendPacket(uint8_t mode, uint16_t x, uint16_t y) {
    char txBuffer[20]; // 调整缓冲区长度以适应字符串格式
    int len;

    // 构造数据包
    len = snprintf(txBuffer, sizeof(txBuffer), "LORA,%d,%d,%d\r", mode, x, y);

    // 发送数据
    for (int i = 0; i < len; i++) {
        while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
        USART_SendData(USART1, txBuffer[i]);
    }
}

// 接收数据包
uint8_t LoRa_ReceivePacket(uint8_t *data, uint16_t *length) {
    // 假设数据包长度存在缓冲区的前两个字节中
    *length = (rxBuffer[0] << 8) | rxBuffer[1];

    if (*length > LORA_RX_BUFFER_SIZE - 2) {
        return 0; // 数据长度错误
    }

    // 复制接收到的数据到用户提供的缓冲区
    memcpy(data, &rxBuffer[2], *length);

    return 1; // 接收成功
}

// 处理接收到的UWB坐标并发送到LoRa
void process_and_send_uwb_coordinates(const char *uwb_coords_str) {
    int x, y;
    char *ptr;

    // 解析x坐标
    x = atoi(uwb_coords_str); // 使用atoi函数将字符串转换为整数
    ptr = strchr(uwb_coords_str, '\0'); // 找到分隔符\0位置
    if (ptr != NULL) {
        // 解析y坐标，ptr+1跳过\0分隔符
        y = atoi(ptr + 1);
        // 发送到LoRa模块
        LoRa_SendPacket(1, x, y);
    }
}

// 在你的接收函数中调用该函数来处理接收到的坐标
void receive_uwb_coordinates_and_send_to_lora(const char *uwb_coords_str) {
    process_and_send_uwb_coordinates(uwb_coords_str);
}
