#ifndef __FLASH_MAP_H
#define __FLASH_MAP_H

#define USER_FLASH_BOOTLOADER_BASE 0x08000000U
#define USER_FLASH_BOOTLOADER_END  0x0800BFFFU
#define USER_FLASH_BOOTLOADER_SIZE 0x0000C000U /* 48k */

#define USER_FLASH_BOOT_DATA_BASE 0x0800C000U
#define USER_FLASH_BOOT_DATA_END  0x0800CFFFU
#define USER_FLASH_BOOT_DATA_SIZE 0x00001000U /* 4k */

#define USER_FLASH_APP_DATA_BASE 0x0800D000U
#define USER_FLASH_APP_DATA_END  0x0800FFFFU
#define USER_FLASH_APP_DATA_SIZE 0x00003000U /* 12k */

#define USER_FLASH_APP_BASE 0x08010000U
#define USER_FLASH_APP_END  0x08027FFFU
#define USER_FLASH_APP_SIZE 0x00018000U /* 96k */

#define USER_FLASH_BIN_BASE 0x08028000U
#define USER_FLASH_BIN_END  0x0803FFFFU
#define USER_FLASH_BIN_SIZE 0x00018000U /* 96k */

#endif /* __FLASH_MAP_H */
