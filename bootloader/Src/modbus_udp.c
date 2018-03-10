#include "stm32f1xx_hal.h"
#include "modbus_crc.h"
#include "modbus_udp.h"
#include "user_flash.h"
#include "spi_lcd.h"
#include "user_uart.h"
#include "user_io.h"
#include "user_boot.h"
#include "stdio.h"
#include <string.h>
#include <stdarg.h>

#define x__PC_MODBUS_
#define __MCU_MODBUS_

#define BigLittleSwap16(A)        ((((uint16_t)(A) & 0xff00) >> 8) | \
                                   (((uint16_t)(A) & 0x00ff) << 8))  
  
  
#define BigLittleSwap32(A)        ((((uint32_t)(A) & 0xff000000) >> 24) | \
                                   (((uint32_t)(A) & 0x00ff0000) >> 8) | \
                                   (((uint32_t)(A) & 0x0000ff00) << 8) | \
                                   (((uint32_t)(A) & 0x000000ff) << 24))  

enum function_code
{
	funcode_pc_connect       = 100,
	funcode_pc_monitor       = 101,
	funcode_pc_read_ram      = 102,
	funcode_pc_write_ram     = 103,
	funcode_pc_read_flash    = 104,
	funcode_pc_write_flash   = 105,
	funcode_pc_read_info     = 106,
	funcode_mcu_read         = 107,
	funcode_mcu_write        = 108,
};

enum modbus_address_id
{
	id_broadcast = 0x00,
	id_reserve   = 0xff
};

#pragma pack(1)

typedef struct
{
	uint8_t high[6];
	uint8_t reserve_0[2];
	uint8_t sp_seat;
	uint8_t sp_env;
	uint8_t reserve_1[6];
	uint8_t reserve_2[255-16];
	uint8_t switch_function;
} ram_t;

typedef union pdu_data
{
	struct pc_connect
	{	
		uint32_t password; /* 连接密码 */
	} pc_connect;
	struct pc_monitor
	{
		uint8_t tick; /* 监控心跳 */
	} pc_monitor;
	struct pc_read_ram
	{
		uint16_t addr;
		uint16_t len;
		uint8_t data[128];
	} pc_read_ram;
	struct pc_write_ram
	{
		uint16_t addr;
		uint16_t len;
		uint8_t data[128];
	} pc_write_ram;
	struct pc_read_flash
	{
		uint16_t addr;
		uint16_t len;
		uint8_t data[128];
	} pc_read_flash;
	struct pc_write_flash
	{
		uint16_t addr;
		uint16_t len;
		uint8_t data[128];
	} pc_write_flash;
	struct pc_read_info
	{
		uint8_t id;
		uint8_t len;
		uint8_t data[128];
	} pc_read_info;
	struct mcu_read
	{
		uint16_t addr;
		uint16_t len;
		uint8_t data[128];
	} mcu_read;
	struct mcu_write
	{
		uint16_t addr;
		uint16_t len;
		uint8_t data[128];
	} mcu_write;
} pdu_data_t;

typedef struct modbus_pdu
{
	uint8_t mbfuncode;
	pdu_data_t data;
} pdu_t;

typedef struct modbus_adu
{
	uint8_t id;
	pdu_t pdu;
	uint16_t crc;
} adu_t;

#pragma pack()

adu_t adu;
uint8_t modbus_buf[1024] = {0};
uint8_t shared_memory_ram[1024] = {0};
uint8_t shared_memory_flash[1024] = {0};
uint8_t shared_memory_pc[1024] = {0};

ram_t *ram = (ram_t *)shared_memory_ram;

#ifdef __PC_MODBUS_
uint16_t mb_req_pc_connect(uint8_t *send_buf)
{
	uint16_t len = 0;
	adu.id = id_reserve;
	len += sizeof(adu.id);
	adu.pdu.mbfuncode = funcode_pc_connect;
	len += sizeof(adu.pdu.mbfuncode);
	adu.pdu.data.pc_connect.password = BigLittleSwap32(456);
	len += sizeof(adu.pdu.data.pc_connect.password);
	adu.crc = crc16((uint8_t *)&adu, len);
	memcpy(send_buf, (uint8_t *)&adu, len);
	send_buf[len] = (uint8_t)(adu.crc);
	len += 1;
	send_buf[len] = (uint8_t)(adu.crc >> 8);
	len += 1;
	return len;
}
static uint8_t pc_send_monitor_tick = 0;
uint16_t mb_req_pc_monitor(uint8_t *send_buf)
{
	uint16_t len = 0;
	adu.id = id_reserve;
	len += sizeof(adu.id);
	adu.pdu.mbfuncode = funcode_pc_monitor;
	len += sizeof(adu.pdu.mbfuncode);
	adu.pdu.data.pc_monitor.tick = pc_send_monitor_tick;
	len += sizeof(adu.pdu.data.pc_monitor.tick);
	adu.crc = crc16((uint8_t *)&adu, len);
	memcpy(send_buf, (uint8_t *)&adu, len);
	send_buf[len] = (uint8_t)(adu.crc);
	len += 1;
	send_buf[len] = (uint8_t)(adu.crc >> 8);
	len += 1;
	return len;
}
static uint16_t pc_send_read_ram_addr = 0;
static uint16_t pc_send_read_ram_len = 0;
uint16_t mb_req_pc_read_ram(uint8_t *send_buf)
{
	uint16_t len = 0;
	adu.id = id_reserve;
	len += sizeof(adu.id);
	adu.pdu.mbfuncode = funcode_pc_read_ram;
	len += sizeof(adu.pdu.mbfuncode);
	adu.pdu.data.pc_read_ram.addr = pc_send_read_ram_addr;
	len += sizeof(adu.pdu.data.pc_read_ram.addr);
	adu.pdu.data.pc_read_ram.len = pc_send_read_ram_len;
	len += sizeof(adu.pdu.data.pc_read_ram.len);
	adu.crc = crc16((uint8_t *)&adu, len);
	memcpy(send_buf, (uint8_t *)&adu, len);
	send_buf[len] = (uint8_t)(adu.crc);
	len += 1;
	send_buf[len] = (uint8_t)(adu.crc >> 8);
	len += 1;
	return len;
}
static uint16_t pc_send_write_ram_addr = 0;
static uint16_t pc_send_write_ram_len = 0;
static uint8_t pc_send_write_ram_data[128];
uint16_t mb_req_pc_write_ram(uint8_t *send_buf)
{
	uint16_t len = 0;
	adu.id = id_reserve;
	len += sizeof(adu.id);
	adu.pdu.mbfuncode = funcode_pc_write_ram;
	len += sizeof(adu.pdu.mbfuncode);
	adu.pdu.data.pc_write_ram.addr = pc_send_write_ram_addr;
	len += sizeof(adu.pdu.data.pc_write_ram.addr);
	adu.pdu.data.pc_write_ram.len = pc_send_write_ram_len;
	len += sizeof(adu.pdu.data.pc_write_ram.len);
	memcpy(adu.pdu.data.pc_write_ram.data, pc_send_write_ram_data, pc_send_write_ram_len);
	len += pc_send_write_ram_len;
	adu.crc = crc16((uint8_t *)&adu, len);
	memcpy(send_buf, (uint8_t *)&adu, len);
	send_buf[len] = (uint8_t)(adu.crc);
	len += 1;
	send_buf[len] = (uint8_t)(adu.crc >> 8);
	len += 1;
	return len;
}
static uint16_t pc_send_read_flash_addr = 0;
static uint16_t pc_send_read_flash_len = 0;
uint16_t mb_req_pc_read_flash(uint8_t *send_buf)
{
	uint16_t len = 0;
	adu.id = id_reserve;
	len += sizeof(adu.id);
	adu.pdu.mbfuncode = funcode_pc_read_flash;
	len += sizeof(adu.pdu.mbfuncode);
	adu.pdu.data.pc_read_flash.addr = pc_send_read_flash_addr;
	len += sizeof(adu.pdu.data.pc_read_flash.addr);
	adu.pdu.data.pc_read_flash.len = pc_send_read_flash_len;
	len += sizeof(adu.pdu.data.pc_read_flash.len);
	adu.crc = crc16((uint8_t *)&adu, len);
	memcpy(send_buf, (uint8_t *)&adu, len);
	send_buf[len] = (uint8_t)(adu.crc);
	len += 1;
	send_buf[len] = (uint8_t)(adu.crc >> 8);
	len += 1;
	return len;
}
static uint16_t pc_send_write_flash_addr = 0;
static uint16_t pc_send_write_flash_len = 0;
static uint8_t pc_send_write_flash_data[128];
uint16_t mb_req_pc_write_flash(uint8_t *send_buf)
{
	uint16_t len = 0;
	adu.id = id_reserve;
	len += sizeof(adu.id);
	adu.pdu.mbfuncode = funcode_pc_write_flash;
	len += sizeof(adu.pdu.mbfuncode);
	adu.pdu.data.pc_write_flash.addr = pc_send_write_flash_addr;
	len += sizeof(adu.pdu.data.pc_write_flash.addr);
	adu.pdu.data.pc_write_flash.len = pc_send_write_flash_len;
	len += sizeof(adu.pdu.data.pc_write_flash.len);
	memcpy(adu.pdu.data.pc_write_flash.data, pc_send_write_flash_data, pc_send_write_flash_len);
	len += pc_send_write_flash_len;
	adu.crc = crc16((uint8_t *)&adu, len);
	memcpy(send_buf, (uint8_t *)&adu, len);
	send_buf[len] = (uint8_t)(adu.crc);
	len += 1;
	send_buf[len] = (uint8_t)(adu.crc >> 8);
	len += 1;
	return len;
}
static uint16_t pc_receive_read_addr = 0;
static uint16_t pc_receive_read_len = 0;
uint16_t mb_rsp_mcu_read(uint8_t *send_buf)
{
	uint16_t len = 0;
	adu.id = id_reserve;
	len += sizeof(adu.id);
	adu.pdu.mbfuncode = funcode_mcu_read;
	len += sizeof(adu.pdu.mbfuncode);
	adu.pdu.data.mcu_read.addr = pc_receive_read_addr;
	len += sizeof(adu.pdu.data.mcu_read.addr);
	adu.pdu.data.mcu_read.len = pc_receive_read_len;
	len += sizeof(adu.pdu.data.mcu_read.len);
	memcpy(adu.pdu.data.mcu_read.data, &shared_memory_pc[pc_receive_read_addr], pc_receive_read_len);
	len += pc_receive_read_len;
	adu.crc = crc16((uint8_t *)&adu, len);
	memcpy(send_buf, (uint8_t *)&adu, len);
	send_buf[len] = (uint8_t)(adu.crc);
	len += 1;
	send_buf[len] = (uint8_t)(adu.crc >> 8);
	len += 1;
	return len;
}
static uint16_t pc_receive_write_addr = 0;
static uint16_t pc_receive_write_len = 0;
static uint8_t pc_receive_write_data[128];
uint16_t mb_rsp_mcu_write(uint8_t *send_buf)
{
	uint16_t len = 0;
	adu.id = id_reserve;
	len += sizeof(adu.id);
	adu.pdu.mbfuncode = funcode_mcu_write;
	len += sizeof(adu.pdu.mbfuncode);
	adu.pdu.data.mcu_write.addr = pc_receive_write_addr;
	len += sizeof(adu.pdu.data.mcu_write.addr);
	adu.pdu.data.mcu_write.len = pc_receive_write_len;
	len += sizeof(adu.pdu.data.mcu_write.len);
	adu.crc = crc16((uint8_t *)&adu, len);
	memcpy(send_buf, (uint8_t *)&adu, len);
	send_buf[len] = (uint8_t)(adu.crc);
	len += 1;
	send_buf[len] = (uint8_t)(adu.crc >> 8);
	len += 1;
	return len;
}
#endif

#ifdef __MCU_MODBUS_
uint16_t mb_rsp_pc_connect(uint8_t *send_buf)
{
	uint16_t len = 0;
	adu.id = id_reserve;
	len += sizeof(adu.id);
	adu.pdu.mbfuncode = funcode_pc_connect;
	len += sizeof(adu.pdu.mbfuncode);
	adu.pdu.data.pc_connect.password = BigLittleSwap32(654);
	len += sizeof(adu.pdu.data.pc_connect.password);
	adu.crc = crc16((uint8_t *)&adu, len);
	memcpy(send_buf, (uint8_t *)&adu, len);
	send_buf[len] = (uint8_t)(adu.crc);
	len += 1;
	send_buf[len] = (uint8_t)(adu.crc >> 8);
	len += 1;
	return len;
}
static uint8_t mcu_receive_monitor_tick = 0;
uint16_t mb_rsp_pc_monitor(uint8_t *send_buf)
{
	uint16_t len = 0;
	adu.id = id_reserve;
	len += sizeof(adu.id);
	adu.pdu.mbfuncode = funcode_pc_monitor;
	len += sizeof(adu.pdu.mbfuncode);
	adu.pdu.data.pc_monitor.tick = mcu_receive_monitor_tick;
	len += sizeof(adu.pdu.data.pc_monitor.tick);
	adu.crc = crc16((uint8_t *)&adu, len);
	memcpy(send_buf, (uint8_t *)&adu, len);
	send_buf[len] = (uint8_t)(adu.crc);
	len += 1;
	send_buf[len] = (uint8_t)(adu.crc >> 8);
	len += 1;
	return len;
}
static uint16_t mcu_receive_read_ram_addr = 0;
static uint16_t mcu_receive_read_ram_len = 0;
uint16_t mb_rsp_pc_read_ram(uint8_t *send_buf)
{
	uint16_t len = 0;
	adu.id = id_reserve;
	len += sizeof(adu.id);
	adu.pdu.mbfuncode = funcode_pc_read_ram;
	len += sizeof(adu.pdu.mbfuncode);
	adu.pdu.data.pc_read_ram.addr = BigLittleSwap16(mcu_receive_read_ram_addr);
	len += sizeof(adu.pdu.data.pc_read_ram.addr);
	adu.pdu.data.pc_read_ram.len = BigLittleSwap16(mcu_receive_read_ram_len);
	len += sizeof(adu.pdu.data.pc_read_ram.len);
	memcpy(adu.pdu.data.pc_read_ram.data, &shared_memory_ram[mcu_receive_read_ram_addr], mcu_receive_read_ram_len);
	len += mcu_receive_read_ram_len;
	adu.crc = crc16((uint8_t *)&adu, len);
	memcpy(send_buf, (uint8_t *)&adu, len);
	send_buf[len] = (uint8_t)(adu.crc);
	len += 1;
	send_buf[len] = (uint8_t)(adu.crc >> 8);
	len += 1;
	return len;
}
static uint16_t mcu_receive_write_ram_addr = 0;
static uint16_t mcu_receive_write_ram_len = 0;
uint16_t mb_rsp_pc_write_ram(uint8_t *send_buf)
{
	uint16_t len = 0;
	adu.id = id_reserve;
	len += sizeof(adu.id);
	adu.pdu.mbfuncode = funcode_pc_write_ram;
	len += sizeof(adu.pdu.mbfuncode);
	adu.pdu.data.pc_write_ram.addr = BigLittleSwap16(mcu_receive_write_ram_addr);
	len += sizeof(adu.pdu.data.pc_write_ram.addr);
	adu.pdu.data.pc_write_ram.len = BigLittleSwap16(mcu_receive_write_ram_len);
	len += sizeof(adu.pdu.data.pc_write_ram.len);
	adu.crc = crc16((uint8_t *)&adu, len);
	memcpy(send_buf, (uint8_t *)&adu, len);
	send_buf[len] = (uint8_t)(adu.crc);
	len += 1;
	send_buf[len] = (uint8_t)(adu.crc >> 8);
	len += 1;
	return len;
}
static uint16_t mcu_receive_read_flash_addr = 0;
static uint16_t mcu_receive_read_flash_len = 0;
uint16_t mb_rsp_pc_read_flash(uint8_t *send_buf)
{
	uint16_t len = 0;
	adu.id = id_reserve;
	len += sizeof(adu.id);
	adu.pdu.mbfuncode = funcode_pc_read_flash;
	len += sizeof(adu.pdu.mbfuncode);
	adu.pdu.data.pc_read_flash.addr = BigLittleSwap16(mcu_receive_read_flash_addr);
	len += sizeof(adu.pdu.data.pc_read_flash.addr);
	adu.pdu.data.pc_read_flash.len = BigLittleSwap16(mcu_receive_read_flash_len);
	len += sizeof(adu.pdu.data.pc_read_flash.len);
	user_flash_read(mcu_receive_read_flash_addr+USER_FLASH_APP_DATA_BASE,
	                mcu_receive_read_flash_len,
	                adu.pdu.data.pc_read_flash.data);
	//memcpy(adu.pdu.data.pc_read_flash.data, &shared_memory_flash[mcu_receive_read_flash_addr], mcu_receive_read_flash_len);
	len += mcu_receive_read_flash_len;
	adu.crc = crc16((uint8_t *)&adu, len);
	memcpy(send_buf, (uint8_t *)&adu, len);
	send_buf[len] = (uint8_t)(adu.crc);
	len += 1;
	send_buf[len] = (uint8_t)(adu.crc >> 8);
	len += 1;
	return len;
}
static uint16_t mcu_receive_write_flash_addr = 0;
static uint16_t mcu_receive_write_flash_len = 0;
uint16_t mb_rsp_pc_write_flash(uint8_t *send_buf)
{
	uint16_t len = 0;
	adu.id = id_reserve;
	len += sizeof(adu.id);
	adu.pdu.mbfuncode = funcode_pc_write_flash;
	len += sizeof(adu.pdu.mbfuncode);
	adu.pdu.data.pc_write_flash.addr = BigLittleSwap16(mcu_receive_write_flash_addr);
	len += sizeof(adu.pdu.data.pc_write_flash.addr);
	adu.pdu.data.pc_write_flash.len = BigLittleSwap16(mcu_receive_write_flash_len);
	len += sizeof(adu.pdu.data.pc_write_flash.len);
	adu.crc = crc16((uint8_t *)&adu, len);
	memcpy(send_buf, (uint8_t *)&adu, len);
	send_buf[len] = (uint8_t)(adu.crc);
	len += 1;
	send_buf[len] = (uint8_t)(adu.crc >> 8);
	len += 1;
	return len;
}
static uint8_t mcu_receive_read_info_id = 0;
static uint8_t mcu_receive_read_info_len = 0;
static uint8_t *mcu_receive_read_info_data = NULL;
uint8_t UUID[] = "000000000000000000000000";
uint16_t mb_rsp_pc_read_info(uint8_t *send_buf)
{
	uint16_t len = 0;
	
	switch (mcu_receive_read_info_id)
	{
		case 0:
			mcu_receive_read_info_len = sizeof(UUID) - 1;
			user_read_uuid(UUID);
		    mcu_receive_read_info_data = UUID;
			break;
		default :
			mcu_receive_read_info_len = 0;
			mcu_receive_read_info_data = NULL;
			break;
	}
	
	adu.id = id_reserve;
	len += sizeof(adu.id);
	adu.pdu.mbfuncode = funcode_pc_read_info;
	len += sizeof(adu.pdu.mbfuncode);
	adu.pdu.data.pc_read_info.id = mcu_receive_read_info_id;
	len += sizeof(adu.pdu.data.pc_read_info.id);
	adu.pdu.data.pc_read_info.len = mcu_receive_read_info_len;
	len += sizeof(adu.pdu.data.pc_read_info.len);
	memcpy(adu.pdu.data.pc_read_info.data, mcu_receive_read_info_data, mcu_receive_read_info_len);
	len += mcu_receive_read_info_len;
	adu.crc = crc16((uint8_t *)&adu, len);
	memcpy(send_buf, (uint8_t *)&adu, len);
	send_buf[len] = (uint8_t)(adu.crc);
	len += 1;
	send_buf[len] = (uint8_t)(adu.crc >> 8);
	len += 1;
	return len;
}
static uint16_t mcu_send_read_addr = 0;
static uint16_t mcu_send_read_len = 0;
uint16_t mb_req_mcu_read(uint8_t *send_buf)
{
	uint16_t len = 0;
	adu.id = id_reserve;
	len += sizeof(adu.id);
	adu.pdu.mbfuncode = funcode_mcu_read;
	len += sizeof(adu.pdu.mbfuncode);
	adu.pdu.data.mcu_read.addr = BigLittleSwap16(mcu_send_read_addr);
	len += sizeof(adu.pdu.data.mcu_read.addr);
	adu.pdu.data.mcu_read.len = BigLittleSwap16(mcu_send_read_len);
	len += sizeof(adu.pdu.data.mcu_read.len);
	adu.crc = crc16((uint8_t *)&adu, len);
	memcpy(send_buf, (uint8_t *)&adu, len);
	send_buf[len] = (uint8_t)(adu.crc);
	len += 1;
	send_buf[len] = (uint8_t)(adu.crc >> 8);
	len += 1;
	return len;
}
static uint16_t mcu_send_write_addr = 0;
static uint16_t mcu_send_write_len = 0;
static uint8_t mcu_send_write_data[128];
uint16_t mb_req_mcu_write(uint8_t *send_buf)
{
	uint16_t len = 0;
	adu.id = id_reserve;
	len += sizeof(adu.id);
	adu.pdu.mbfuncode = funcode_mcu_write;
	len += sizeof(adu.pdu.mbfuncode);
	adu.pdu.data.mcu_write.addr = BigLittleSwap16(mcu_send_write_addr);
	len += sizeof(adu.pdu.data.mcu_write.addr);
	adu.pdu.data.mcu_write.len = BigLittleSwap16(mcu_send_write_len);
	len += sizeof(adu.pdu.data.mcu_write.len);
	memcpy(adu.pdu.data.mcu_write.data, mcu_send_write_data, mcu_send_write_len);
	len += mcu_send_write_len;
	adu.crc = crc16((uint8_t *)&adu, len);
	memcpy(send_buf, (uint8_t *)&adu, len);
	send_buf[len] = (uint8_t)(adu.crc);
	len += 1;
	send_buf[len] = (uint8_t)(adu.crc >> 8);
	len += 1;
	return len;
}
#endif

uint8_t check_crc(uint8_t *adu, uint16_t len)
{
	uint16_t crc16_value;
	if (adu != NULL)
	{
		if (len >= 2)
		{
			crc16_value = (uint16_t)(adu[len - 2] | adu[len - 1] << 8);
			if (crc16_value == crc16(adu, (len - 2)))
			{
				return 1;
			}
		}
	}
	return 0;
}

uint8_t get_address(uint8_t *adu, uint16_t len)
{
	if (adu != NULL)
	{
		if (len > 0)
		{
			return adu[0];
		}
	}
	return 0;
}

uint8_t get_funcode(uint8_t *adu, uint16_t len)
{
	if (adu != NULL)
	{
		if (len > 1)
		{
			return adu[1];
		}
	}
	return 0;
}

uint16_t get_data(uint8_t *adu, uint16_t len, uint8_t *data)
{
	if (adu != NULL)
	{
		if (len > 4)
		{
			memcpy(data, (adu+2), (len - 4));
			return (len - 4);
		}
	}
	return 0;
}

uint32_t get_password(uint8_t *adu, uint16_t len)
{
	uint8_t data[128];
	get_data(adu, len, data);
	return *((uint32_t *)data);
}

uint8_t get_read_ram_cmd(uint8_t *adu, uint16_t len)
{
	uint8_t data[128];
	uint16_t read_addr;
	uint16_t read_len;
	get_data(adu, len, data);
	read_addr = *((uint16_t *)data);
	read_addr = BigLittleSwap16(read_addr);
	read_len = *((uint16_t *)(&data[2]));
	read_len = BigLittleSwap16(read_len);
	mcu_receive_read_ram_addr = read_addr;
	mcu_receive_read_ram_len = read_len;
	return 1;
}

uint8_t get_write_ram_cmd(uint8_t *adu, uint16_t len)
{
	uint8_t data[128];
	uint16_t write_addr;
	uint16_t write_len;
	get_data(adu, len, data);
	write_addr = *((uint16_t *)data);
	write_addr = BigLittleSwap16(write_addr);
	write_len = *((uint16_t *)(&data[2]));
	write_len = BigLittleSwap16(write_len);
	mcu_receive_write_ram_addr = write_addr;
	mcu_receive_write_ram_len = write_len;
	memcpy(&shared_memory_ram[mcu_receive_write_ram_addr], &data[4], mcu_receive_write_ram_len);
	return 1;
}

uint8_t get_read_flash_cmd(uint8_t *adu, uint16_t len)
{
	uint8_t data[128];
	uint16_t read_addr;
	uint16_t read_len;
	get_data(adu, len, data);
	read_addr = *((uint16_t *)data);
	read_addr = BigLittleSwap16(read_addr);
	read_len = *((uint16_t *)(&data[2]));
	read_len = BigLittleSwap16(read_len);
	mcu_receive_read_flash_addr = read_addr;
	mcu_receive_read_flash_len = read_len;
	return 1;
}

uint8_t get_write_flash_cmd(uint8_t *adu, uint16_t len)
{
	uint8_t data[128];
	uint16_t write_addr;
	uint16_t write_len;
	get_data(adu, len, data);
	write_addr = *((uint16_t *)data);
	write_addr = BigLittleSwap16(write_addr);
	write_len = *((uint16_t *)(&data[2]));
	write_len = BigLittleSwap16(write_len);
	mcu_receive_write_flash_addr = write_addr;
	mcu_receive_write_flash_len = write_len;
	user_flash_write(mcu_receive_write_flash_addr+USER_FLASH_APP_DATA_BASE,
                     mcu_receive_write_flash_len,
	                 &data[4]);
	//memcpy(&shared_memory_flash[mcu_receive_write_flash_addr], &data[4], mcu_receive_write_flash_len);
	return 1;
}

uint8_t get_read_info_cmd(uint8_t *adu, uint16_t len)
{
	uint8_t data[128];
	uint8_t read_id;
	uint8_t read_len;
	get_data(adu, len, data);
	read_id = data[0];
	mcu_receive_read_info_id = read_id;
	return 1;
}

uint8_t get_monitor_tick(uint8_t *adu, uint16_t len)
{
	uint8_t data[1] = {0};
	if (adu != NULL)
	{
		if (len == 5)
		{
			get_data(adu, len, data);
			mcu_receive_monitor_tick = data[0];
			return 1;
		}
	}
	return 0;
}

uint16_t mb_rsp_connect(uint8_t *adu, uint16_t len, uint8_t *out)
{	
	uint8_t address;
	uint8_t function_code;
	
	if (check_crc(adu, len) != 0)
	{
		address = get_address(adu, len);
		switch (address)
		{
			case id_reserve:
				function_code = get_funcode(adu, len);
				switch (function_code)
				{
					case funcode_pc_connect:
						if (get_password(adu, len) == BigLittleSwap32(456))
						{
							return mb_rsp_pc_connect(out);
						}
						break;
					default :
						break;
				}
				break;
			default :
				break;
		}
	}
	
	return 0;
}

uint16_t mb_rsp(uint8_t *adu, uint16_t len, uint8_t *out)
{	
	uint8_t address;
	uint8_t function_code;
	
	if (check_crc(adu, len) != 0)
	{
		address = get_address(adu, len);
		switch (address)
		{
			case id_broadcast:
				function_code = get_funcode(adu, len);
				switch (function_code)
				{
					case funcode_pc_write_ram:
						if (get_write_ram_cmd(adu, len))
						{
							return 0;
						}
						break;
					default :
						break;
				}
				break;
			case id_reserve:
				function_code = get_funcode(adu, len);
				switch (function_code)
				{
					case funcode_pc_monitor:
						if (get_monitor_tick(adu, len))
						{
							return mb_rsp_pc_monitor(out);
						}
						break;
					case funcode_pc_read_ram:
						if (get_read_ram_cmd(adu, len))
						{
							return mb_rsp_pc_read_ram(out);
						}
						break;
					case funcode_pc_write_ram:
						if (get_write_ram_cmd(adu, len))
						{
							return mb_rsp_pc_write_ram(out);
						}
						break;
					case funcode_pc_read_flash:
						if (get_read_flash_cmd(adu, len))
						{
							return mb_rsp_pc_read_flash(out);
						}
						break;
					case funcode_pc_write_flash:
						if (get_write_flash_cmd(adu, len))
						{
							return mb_rsp_pc_write_flash(out);
						}
						break;
					case funcode_pc_read_info:
						if (get_read_info_cmd(adu, len))
						{
							return mb_rsp_pc_read_info(out);
						}
						break;
					default :
						break;
				}
				break;
			default :
				break;
		}
	}
	
	return 0;
}

enum
{
	FUNCTION_NORMAL = 0,
	FUNCTION_FIRMWARE_UPDATE = 1
};

void modbus_udp_task(void)
{
	static uint8_t flag_first_run = 1;
	if (flag_first_run)
	{
		flag_first_run = 0;
		ram->high[0] = 32;
		ram->high[1] = 64;
		ram->high[2] = 128;
	}
	lcd_high_update(ram->high[0], ram->high[1], ram->high[2]);
}

void modbus_switch_function_task(void)
{
	if (ram->switch_function == FUNCTION_FIRMWARE_UPDATE)
	{
		boot_set_update_flag();
		HAL_NVIC_SystemReset();
		while(1);
	}
}


