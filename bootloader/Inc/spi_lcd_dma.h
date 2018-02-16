#ifndef __SPI_LCD_DMA_H
#define __SPI_LCD_DMA_H

#include "stm32f1xx_hal.h"
#include "user_config.h"
#include "user_spi.h"

#define fb_write_point(x, y, rgb565) _fb_write_point((x), (y), BSWAP_16(rgb565))
#define fb_fill(color) _fb_fill(BSWAP_16(color))  

extern uint16_t fb_fps;

extern void fb_init(void);
extern void _fb_write_point(uint16_t x,uint16_t y,uint16_t rgb565);
extern uint16_t fb_read_point(uint16_t x,uint16_t y);
extern void _fb_fill(uint16_t color);
extern void fb_update(void);
extern void fb_printf(const char *fmt /*format*/, ...);
	
#endif /* __SPI_LCD_DMA_H */
