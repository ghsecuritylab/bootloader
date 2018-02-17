#include "stm32f1xx_hal.h"
#include "user_config.h"
#include "user_flash.h"
#include <string.h>

typedef struct flash_write_buf
{
	uint64_t buf[FLASH_PAGE_SIZE/8];
} flash_write_t;

static uint8_t flash_page_buf[FLASH_PAGE_SIZE] = {0};
static flash_write_t *flash_write = (flash_write_t *)flash_page_buf;

void user_flash_read(uint32_t addr, uint16_t len, uint8_t *data)
{
	memcpy(data, (uint8_t *)addr, len);
}

void user_flash_write(uint32_t addr, uint16_t len, uint8_t *data)
{
	uint32_t flash_addr;
	flash_addr = addr/FLASH_PAGE_SIZE*FLASH_PAGE_SIZE;
	int i;
	
	//读出全部flash data
	user_flash_read(flash_addr, FLASH_PAGE_SIZE, flash_page_buf);
	//拷贝需要更新的flash data
	memcpy((flash_page_buf+(addr-flash_addr)), data, len);
	//1、解锁FLASH
	HAL_FLASH_Unlock();

	//2、擦除FLASH
	//初始化FLASH_EraseInitTypeDef
	FLASH_EraseInitTypeDef f;
	f.TypeErase = FLASH_TYPEERASE_PAGES;
	f.PageAddress = flash_addr;
	f.NbPages = 1;
	//设置PageError
	uint32_t PageError = 0;
	//调用擦除函数
	HAL_FLASHEx_Erase(&f, &PageError);

	//3、对FLASH烧写
	for (i=0; i<(FLASH_PAGE_SIZE/8); i++)
	{
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, flash_addr, flash_write->buf[i]);
		flash_addr += 8;
	}
	//4、锁住FLASH
	HAL_FLASH_Lock();
}

void hex_to_str(uint8_t *str, uint8_t *hex, uint16_t len)
{
	uint8_t hex_low,hex_high;
	uint16_t i;

	for (i=0; i<len; i++)
	{
		hex_high = 0x30 + (hex[i]>>4);
		hex_low = 0x30 + (hex[i]&0x0F);
		if (hex_high > 57)
		{
			hex_high += + 7;
		}
		if (hex_low > 57)
		{
			hex_low += + 7;
		}
		str[i*2] = hex_high;
		str[i*2+1] = hex_low;
	}

	str[len*2] = '\0';
}

void user_read_uuid(uint8_t *str)
{
	uint32_t uuid[3] = {0};
	
    uuid[0] = *(__IO uint32_t *)(0x1ffff7e8);
    uuid[1] = *(__IO uint32_t *)(0x1ffff7ec);
    uuid[2] = *(__IO uint32_t *)(0x1ffff7f0);
	
	hex_to_str(str, (uint8_t *)uuid, sizeof(uuid));
}
