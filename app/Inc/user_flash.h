#ifndef __USER_FLASH_H
#define __USER_FLASH_H

#include "stm32f1xx_hal.h"

#define USER_FLASH_BOOTLOADER_BASE 0x08000000U
#define USER_FLASH_BOOTLOADER_END  0x0800BFFFU
#define USER_FLASH_BOOTLOADER_SIZE 0x0000C000U /* 48k */

#define USER_FLASH_DATA_BASE 0x0000C000U
#define USER_FLASH_DATA_END  0x0800FFFFU
#define USER_FLASH_DATA_SIZE 0x00004000U /* 16k */

#define USER_FLASH_APP_BASE 0x08010000U
#define USER_FLASH_APP_END  0x08027FFFU
#define USER_FLASH_APP_SIZE 0x00018000U /* 96k */

#define USER_FLASH_BIN_BASE 0x08028000U
#define USER_FLASH_BIN_END  0x0803FFFFU
#define USER_FLASH_BIN_SIZE 0x00018000U /* 96k */

extern void user_flash_read(uint16_t addr, uint16_t len, uint8_t *data);
extern void user_flash_write(uint16_t addr, uint16_t len, uint8_t *data);
extern void user_read_uuid(uint8_t *str);

#endif /* __USER_FLASH_H */
