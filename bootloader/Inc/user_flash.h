#ifndef __USER_FLASH_H
#define __USER_FLASH_H

#include "stm32f1xx_hal.h"
#include "flash_map.h"

extern void user_flash_read(uint32_t addr, uint16_t len, uint8_t *data);
extern void user_flash_write(uint32_t addr, uint16_t len, uint8_t *data);
extern void user_read_uuid(uint8_t *str);

#endif /* __USER_FLASH_H */
