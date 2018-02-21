#include "stm32f1xx_hal.h"
#include "user_config.h"
#include "user_spi.h"

#ifdef USE_SPI_LCD_DMA
uint8_t flag_spi_tx_cplt = 1;
#endif

void user_spi_init(void)
{
	uint8_t spi_tx_data = 0xff;
	/* TODO: can not without timeout */
	(void)HAL_SPI_Transmit(&hspi1, &spi_tx_data, 1, HAL_MAX_DELAY);
}

void spi1_write_byte(uint8_t tx_data)
{	
	uint8_t spi_tx_data = 0;
	
	spi_tx_data = tx_data;
	/* TODO: can not without timeout */
	(void)HAL_SPI_Transmit(&hspi1, &spi_tx_data, 1, HAL_MAX_DELAY);		    
}

#ifdef USE_SPI_LCD_DMA
void spi1_write_data(uint8_t *tx_data, uint16_t len)
{	
	/* TODO: can not without timeout */
	flag_spi_tx_cplt = 0;
	(void)HAL_SPI_Transmit_DMA(&hspi1, tx_data, len);
}
	
void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hspi);
  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_SPI_TxCpltCallback should be implemented in the user file
  */
	flag_spi_tx_cplt = 1;
}
#endif
