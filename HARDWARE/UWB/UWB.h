#ifndef __UWB_H
#define __UWB_H

#include "stm32f10x_usart.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"

// 定义基站坐标
#define BASE1_X 0
#define BASE1_Y 0
#define BASE2_X 320
#define BASE2_Y 0
#define BASE3_X 320
#define BASE3_Y 320

// 外部全局变量声明
extern uint16_t uwb_x;
extern uint16_t uwb_y;

// 函数声明
void UWB_Init(void);
void UWB_SendData(uint8_t data);
void USART_SendString(char* str);
uint8_t UWB_ReceiveData(void);
void UWB_GetCoordinates(void);
void USART1_Init(void);
void USART1_IRQHandler(void);

#endif /* __UWB_H */
