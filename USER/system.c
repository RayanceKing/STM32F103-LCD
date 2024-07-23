#include "system.h"
#include "stm32f10x.h"

void system_init(void) {
    // ????
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);

    // ??????
    SystemInit();

    // ??GPIO???
    MX_GPIO_Init();
    MX_USART1_UART_Init();
    MX_SPI1_Init();
}

void MX_GPIO_Init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;

    // ??GPIO??
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; // ??
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void MX_USART1_UART_Init(void) {
    USART_InitTypeDef USART_InitStructure;

    // ??USART1
    USART_InitStructure.USART_BaudRate = 9600;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    USART_Init(USART1, &USART_InitStructure);

    // ??USART1
    USART_Cmd(USART1, ENABLE);
}

void MX_SPI1_Init(void) {
    SPI_InitTypeDef SPI_InitStructure;

    // ??SPI1
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init(SPI1, &SPI_InitStructure);

    // ??SPI1
    SPI_Cmd(SPI1, ENABLE);
}

void Error_Handler(void) {
    // ??????
    while (1) {
        // ??????
    }
}
