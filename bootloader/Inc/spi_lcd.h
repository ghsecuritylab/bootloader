#ifndef __SPI_LCD_H
#define __SPI_LCD_H

#include "stm32f1xx_hal.h"
#include "user_config.h"
#include "user_spi.h"
#include "spi_lcd_dma.h"

#define X_MAX_PIXEL	        128
#define Y_MAX_PIXEL	        160

#define RED  	0xf800
#define GREEN	0x07e0
#define BLUE 	0x001f
#define WHITE	0xffff
#define BLACK	0x0000
#define YELLOW  0xFFE0
#define GRAY0   0xEF7D   	//»ÒÉ«0 3165 00110 001011 00101
#define GRAY1   0x8410      	//»ÒÉ«1      00000 000000 00000
#define GRAY2   0x4208      	//»ÒÉ«2  1111111111011111

#ifdef USE_SPI_LCD_DMA
#define LCD_UsrLog(...) \
do{ \
	fb_printf(__VA_ARGS__); \
	printf(__VA_ARGS__); \
}while(0)
#else
#define LCD_UsrLog(...) \
do{ \
	lcd_printf(__VA_ARGS__); \
	printf(__VA_ARGS__); \
}while(0)
#endif

#ifdef USE_SPI_LCD_DMA
#define LCD_ErrLog(...) \
do{ \
	fb_printf(__VA_ARGS__); \
	printf(__VA_ARGS__); \
}while(0)
#else
#define LCD_ErrLog(...) \
do{ \
	lcd_printf(__VA_ARGS__); \
	printf(__VA_ARGS__); \
}while(0)
#endif

extern const unsigned char asc16[];

extern void spi_lcd_init(void);
extern void lcd_test(void);
extern void lcd_printf(const char *fmt /*format*/, ...);
extern void lcd_high_update(uint8_t display_high1, uint8_t display_high2, uint8_t display_high3);

extern void Lcd_SetRegion(uint16_t x_start,uint16_t y_start,uint16_t x_end,uint16_t y_end);
extern void Lcd_WriteIndex(uint8_t Index);

#endif /* __SPI_LCD_H */
