#ifndef __MODBUS_UDP_H
#define __MODBUS_UDP_H

#include "stm32f1xx_hal.h"
#include "user_config.h"

extern void modbus_udp_task(void);
extern uint16_t mb_rsp_connect(uint8_t *adu, uint16_t len, uint8_t *out);
extern uint16_t mb_rsp(uint8_t *adu, uint16_t len, uint8_t *data);

extern uint16_t send_ltc(uint8_t *send_buf);
extern void modbus_bus485_task(void);
extern void modbus_switch_function_task(void);

#endif /* __MODBUS_UDP_H */
