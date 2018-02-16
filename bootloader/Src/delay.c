#include "stm32f1xx_hal.h"
#include "user_config.h"
#include "delay.h"


void delay_ms(uint16_t nms)
{	 
	HAL_Delay(nms);    
} 
