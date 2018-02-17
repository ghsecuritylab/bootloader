#include "stm32f1xx_hal.h"
#include "user_config.h"
#include "user_boot.h"
#include "app_bin.h"
#include "user_flash.h"
#include "xmodem.h"
#include <string.h>

const uint8_t app_bin[] __attribute__((at(USER_FLASH_APP_BASE))) = {APP_BIN};
typedef void (*pFunction)(void);

void user_boot2app(void)
{
	uint32_t JumpAddress;
	uint32_t ApplicationAddress = USER_FLASH_APP_BASE;
	pFunction Jump_To_Application;
	if (((*(__IO uint32_t*)ApplicationAddress) & 0x2FFE0000 ) == 0x20000000)  
	{   
		JumpAddress = *(__IO uint32_t*) (ApplicationAddress + 4);  
		Jump_To_Application = (pFunction) JumpAddress;  
	 
		__set_MSP(*(__IO uint32_t*) ApplicationAddress);  
		Jump_To_Application();  
	}  
}

void user_boot(void)
{
	if (iot_xmodem_update_fw() == 0)
	{
		user_boot2app();
	}
}
