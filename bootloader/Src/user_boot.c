#include "stm32f1xx_hal.h"
#include "user_config.h"
#include "user_boot.h"
//#include "app_bin.h"
#include "user_flash.h"
#include "xmodem.h"
#include <string.h>

#pragma pack(1)
typedef struct boot_status
{
	bool update_flag;
} boot_status_t;
#pragma pack()

boot_status_t *boot_status = (boot_status_t *)USER_FLASH_BOOT_DATA_BASE;
//const uint8_t app_bin[] __attribute__((at(USER_FLASH_APP_BASE))) = {APP_BIN};
//const boot_status_t boot_status_init __attribute__((at(USER_FLASH_BOOT_DATA_BASE))) = {
//.update_flag = false
//};
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
//	if (iot_xmodem_update_fw() == 0)
	if (boot_status->update_flag == false)
	{
		user_boot2app();
	}
	else
	{
		boot_clean_update_flag();
	}
}

void boot_set_update_flag(void)
{
	bool update_flag;
	update_flag = true;
	user_flash_write((uint32_t)&(boot_status->update_flag),
	                 sizeof(update_flag),
				     (uint8_t *)&(update_flag));
}

void boot_clean_update_flag(void)
{
	bool update_flag;
	update_flag = false;
	user_flash_write((uint32_t)&(boot_status->update_flag),
	                 sizeof(update_flag),
				     (uint8_t *)&(update_flag));
}
