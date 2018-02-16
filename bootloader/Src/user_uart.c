#include "stm32f1xx_hal.h"
#include "user_uart.h"

#include <string.h>

extern UART_HandleTypeDef huart2;

static uint8_t uart2_receive_data = 0U;

void user_uart_init(void)
{
	HAL_UART_Receive_IT(&huart2, (uint8_t *)&uart2_receive_data, 1);	//串口接收一个字节，并通过中断返回结果
}

extern void lcd_printf(const char *fmt /*format*/, ...);

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if (huart == &huart2)
	{
		huart2.Instance->DR = uart2_receive_data;
		while(__HAL_UART_GET_FLAG(&huart2, UART_FLAG_TXE) == RESET);
		HAL_UART_Receive_IT(&huart2, (uint8_t *)&uart2_receive_data, 1);
	}
}

int fputc(int ch, FILE *f)
{
	huart2.Instance->DR = ch;
	while(__HAL_UART_GET_FLAG(&huart2, UART_FLAG_TXE) == RESET);
	return ch; 
}
