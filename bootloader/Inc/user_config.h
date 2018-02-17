#ifndef __USER_CONFIG_H
#define __USER_CONFIG_H

#include "stm32f1xx_hal.h"
#include <stdbool.h>

#define ABS(x) ((x)>0?(x):-(x)) 

/* atomic instructions */
#define SAFE(x) do{ \
	__set_PRIMASK(1); \
	x; \
	__set_PRIMASK(0); \
}while(0)

#define BSWAP_16(x) \
(uint16_t)((((uint16_t)(x) & 0x00ff) << 8) | \
           (((uint16_t)(x) & 0xff00) >> 8))

extern UART_HandleTypeDef huart2;
extern SPI_HandleTypeDef hspi1;
extern DMA_HandleTypeDef hdma_spi1_tx;

#endif /* __USER_CONFIG_H */
