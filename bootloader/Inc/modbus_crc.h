#ifndef __MODBUS_CRC_H
#define __MODBUS_CRC_H

#include "stm32f1xx_hal.h"
#include "user_config.h"

extern uint16_t crc16(uint8_t *data, uint16_t len);

#endif /* __MODBUS_CRC_H */
