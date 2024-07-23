#ifndef __LORA_H
#define __LORA_H

#include "stm32f10x.h"

// 定义 LoRa 使用的 USART
#define LORA_UART USART1

// 定义缓冲区大小
#define LORA_TX_BUFFER_SIZE 256
#define LORA_RX_BUFFER_SIZE 256

// LoRa 配置结构体
typedef struct {
    uint32_t frequency;       // 频率
    uint8_t spreadingFactor;  // 扩频因子
    uint8_t bandwidth;        // 带宽
    uint8_t codingRate;       // 编码率
    uint8_t power;            // 发射功率
} LoRa_ConfigTypeDef;

// 函数声明
void USART1_Init(void);
void LoRa_Init(LoRa_ConfigTypeDef *config);
void LoRa_SetTxMode(void);
void LoRa_SetRxMode(void);
//uint8_t LoRa_SendPacket(uint8_t mode, uint16_t x, uint16_t y);
uint8_t LoRa_ReceivePacket(uint8_t *data, uint16_t *length);
void LoRa_SendPacket(uint8_t mode, uint16_t x, uint16_t y);
// 在接收函数中调用该函数来处理接收到的坐标并发送到LoRa
void receive_uwb_coordinates_and_send_to_lora(const char *uwb_coords_str);
void process_and_send_uwb_coordinates(const char *uwb_coords_str);
#endif // __LORA_H
