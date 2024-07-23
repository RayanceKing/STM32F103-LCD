#ifndef SYSTEM_H
#define SYSTEM_H

void system_init(void);
void MX_GPIO_Init(void);
void MX_USART1_UART_Init(void);
void MX_SPI1_Init(void);
void Error_Handler(void);

#endif // SYSTEM_H
