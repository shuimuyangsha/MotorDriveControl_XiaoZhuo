#ifndef __sfr_H
#define __sfr_H

///////////////////////WIRED//////////////////////////
#define LEFT_UART USART1
#define LEFT_H_UART huart1

#define RIGHT_UART USART2
#define RIGHT_H_UART huart2


#define COM485_UART USART1

#define TX_485_EN_Pin GPIO_PIN_11
#define TX_485_EN_GPIO_Port GPIOA

#define Enable485TX() 	do{COM485_UART->CR1 &= (~UART_IT_RXNE); HAL_GPIO_WritePin(TX_485_EN_GPIO_Port,TX_485_EN_Pin,  GPIO_PIN_SET); USART1->CR1 |= USART_CR1_TE; USART1->CR1 &= ~USART_CR1_RE;}while(0)
#define Disable485TX() 	do{HAL_GPIO_WritePin(TX_485_EN_GPIO_Port,TX_485_EN_Pin,  GPIO_PIN_RESET);USART1->CR1 |= USART_CR1_RE; USART1->CR1 &= ~USART_CR1_TE;COM485_UART->CR1 |= UART_IT_RXNE;}while(0)



#endif
