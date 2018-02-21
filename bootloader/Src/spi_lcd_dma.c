#include "stm32f1xx_hal.h"
#include "user_config.h"

#ifdef USE_SPI_LCD_DMA

#include "user_io.h"
#include "spi_lcd.h"
#include "spi_lcd_dma.h"
#include "delay.h"
#include "stdio.h"
#include <string.h>
#include <stdarg.h>

uint16_t frame_buffer[X_MAX_PIXEL*Y_MAX_PIXEL] = {0};

uint16_t fb_fps = 0u;
uint32_t fb_fps_timer = 0u;
uint32_t fb_update_timer = 0u;

void fb_init(void)
{
	fb_fill(BLACK);
}

void _fb_write_point(uint16_t x,uint16_t y,uint16_t color)
{
	frame_buffer[X_MAX_PIXEL*y+x] = color;
}

uint16_t fb_read_point(uint16_t x,uint16_t y)
{
	return frame_buffer[X_MAX_PIXEL*y+x];
}

void _fb_fill(uint16_t color)               
{	
	unsigned int i;
	for(i=0; i<(X_MAX_PIXEL*Y_MAX_PIXEL); i++)
	{
		frame_buffer[i] = color;
	}	
}

void fb_string_display(uint16_t x, uint16_t y, uint16_t fc, uint16_t bc, uint8_t *s)
{
	unsigned char i,j;
	unsigned short k,x0;
	x0=x;

	while(*s) 
	{	
		if((*s) < 128) 
		{
			k=*s;
			if (k==13) 
			{
				x=x0;
				y+=16;
			}
			else 
			{
				if (k>32) k-=32; else k=0;
	
			    for(i=0;i<16;i++)
				for(j=0;j<8;j++) 
					{
				    	if(asc16[k*16+i]&(0x80>>j))	fb_write_point(x+j,y+i,fc);
						else 
						{
							if (fc!=bc) fb_write_point(x+j,y+i,bc);
						}
					}
				x+=8;
			}
			s++;
		}
		else
		{
			s++;
		}
	}
}

void fb_string_display_full_lcd(uint16_t fc, uint16_t bc, uint8_t *s)
{
	unsigned char i,j;
	unsigned short k,x0;
	unsigned char x_index = 0;
	uint16_t x = 0;
	uint16_t y = 0;
	x0=x;

	while(*s) 
	{	
		if((*s) < 128) 
		{
			k=*s;
			if (k==13)
			{
				x_index = 0;
				x=x0;
				y+=16;
			}
			else 
			{
				if (x_index == 15)
				{
					x_index = 0;
					x=x0;
					y+=16;
				}
				if (k>32) k-=32; else k=0;
	
			    for(i=0;i<16;i++)
				{
					for(j=0;j<8;j++) 
					{
				    	if(asc16[k*16+i]&(0x80>>j))
						{
							fb_write_point(x+j,y+i,fc);
						}
						else 
						{
							if (fc!=bc)
							{
								fb_write_point(x+j,y+i,bc);
							}
						}
					}
				}
				x+=8;
			}
			s++;
			x_index++;
		}
		else
		{
			s++;
		}
	}
}

void fb_printf(const char *fmt /*format*/, ...)
{
	va_list ap;
	uint8_t fb_str[256] = {0};
	va_start(ap, fmt);
	vsprintf((char *)fb_str, fmt, ap);
	fb_fill(BLACK);
    fb_string_display_full_lcd(WHITE,BLACK,fb_str);
	va_end(ap);
}

bool _fb_update(void)               
{	
	if (flag_spi_tx_cplt == 0)
		return false;
	Lcd_SetRegion(0,0,X_MAX_PIXEL-1,Y_MAX_PIXEL-1);
	Lcd_WriteIndex(0x2C);
	LCD_CS_CLR();
	LCD_DC_SET();
	spi1_write_data((uint8_t *)frame_buffer, sizeof(frame_buffer));
	return true;
}

void fb_printf_fps(void)
{
	uint8_t fb_str[256] = {0};
	sprintf((char *)fb_str, "FPS:%d", fb_fps);
    fb_string_display(0,9*16,WHITE,BLACK,fb_str);
}

void fb_update(void)               
{	
	if (HAL_GetTick() - fb_update_timer >= 50)
	{
		fb_update_timer =  HAL_GetTick();
		fb_printf_fps();
		if(_fb_update())
		{
			fb_fps = 1000/(HAL_GetTick() - fb_fps_timer);
			fb_fps_timer = HAL_GetTick();
		}
	}
}
#endif
