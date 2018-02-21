#ifndef __USER_SPI_H
#define __USER_SPI_H

#include "stm32f1xx_hal.h"
#include "user_config.h"

#ifdef USE_SPI_LCD_DMA
extern uint8_t flag_spi_tx_cplt;
#endif

extern void user_spi_init(void);

extern void spi1_write_byte(uint8_t tx_data);
#ifdef USE_SPI_LCD_DMA
extern void spi1_write_data(uint8_t *tx_data, uint16_t len);
#endif

#endif /* __USER_SPI_H */
