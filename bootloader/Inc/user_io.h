#ifndef __USER_IO_H
#define __USER_IO_H

#include "stm32f1xx_hal.h"

#define LED1_TOGGLE() HAL_GPIO_TogglePin(OUTPUT_LED1_GPIO_Port, OUTPUT_LED1_Pin)
#define LED2_TOGGLE() HAL_GPIO_TogglePin(OUTPUT_LED2_GPIO_Port, OUTPUT_LED2_Pin)
#define LED3_TOGGLE() HAL_GPIO_TogglePin(OUTPUT_LED3_GPIO_Port, OUTPUT_LED3_Pin)

#define LED1_ON() HAL_GPIO_WritePin(OUTPUT_LED1_GPIO_Port, OUTPUT_LED1_Pin, GPIO_PIN_RESET)
#define LED2_ON() HAL_GPIO_WritePin(OUTPUT_LED2_GPIO_Port, OUTPUT_LED2_Pin, GPIO_PIN_RESET)
#define LED3_ON() HAL_GPIO_WritePin(OUTPUT_LED3_GPIO_Port, OUTPUT_LED3_Pin, GPIO_PIN_RESET)

#define LED1_OFF() HAL_GPIO_WritePin(OUTPUT_LED1_GPIO_Port, OUTPUT_LED1_Pin, GPIO_PIN_SET)
#define LED2_OFF() HAL_GPIO_WritePin(OUTPUT_LED2_GPIO_Port, OUTPUT_LED2_Pin, GPIO_PIN_SET)
#define LED3_OFF() HAL_GPIO_WritePin(OUTPUT_LED3_GPIO_Port, OUTPUT_LED3_Pin, GPIO_PIN_SET)

#define SPI_LCD_CS(x) HAL_GPIO_WritePin(SPI_LCD_CS_GPIO_Port, SPI_LCD_CS_Pin, (GPIO_PinState)(x))
#define OUTPUT_LCD_DC(x) HAL_GPIO_WritePin(OUTPUT_LCD_DC_GPIO_Port, OUTPUT_LCD_DC_Pin, (GPIO_PinState)(x))
#define OUTPUT_LCD_BL(x) HAL_GPIO_WritePin(OUTPUT_LCD_BL_GPIO_Port, OUTPUT_LCD_BL_Pin, (GPIO_PinState)(x))
#define OUTPUT_LCD_RES(x) HAL_GPIO_WritePin(OUTPUT_LCD_RES_GPIO_Port, OUTPUT_LCD_RES_Pin, (GPIO_PinState)(x))

#define LCD_CS_CLR() SPI_LCD_CS(0)
#define LCD_CS_SET() SPI_LCD_CS(1)

#define LCD_DC_CLR() OUTPUT_LCD_DC(0)
#define LCD_DC_SET() OUTPUT_LCD_DC(1)

#define LCD_RST_CLR() OUTPUT_LCD_RES(0)
#define LCD_RST_SET() OUTPUT_LCD_RES(1)

#define LCD_LED_CLR() OUTPUT_LCD_BL(0)
#define LCD_LED_SET() OUTPUT_LCD_BL(1)

extern void user_io_init(void);

#endif /* __USER_IO_H */
