/**
  ******************************************************************************
  * @file    LwIP/LwIP_UDP_Echo_Client/Src/udp_echoclient.c
  * @author  MCD Application Team
  * @version V1.6.0
  * @date    12-May-2017
  * @brief   UDP echo client
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright © 2016 STMicroelectronics International N.V. 
  * All rights reserved.</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "lwip/pbuf.h"
#include "lwip/udp.h"
#include "lwip/tcp.h"
#include "app_ethernet.h"
#include "spi_lcd.h"
#include "modbus_udp.h"
#include "user_time.h"
#include "user_io.h"
#include <string.h>
#include <stdio.h>

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
#define UDP_SEND_TIMER_MSECS    200
/* Private variables ---------------------------------------------------------*/
extern struct netif gnetif;
/* Private function prototypes -----------------------------------------------*/
void udp_receive_callback(void *arg, struct udp_pcb *upcb, struct pbuf *p, const ip_addr_t *addr, u16_t port);

uint32_t UDPSendTimer = 0u;
uint8_t udp_send_active_flag = 0u;

uint8_t udp_send_buf[1024];
__IO uint8_t message_count = 0;
struct udp_pcb *upcb_client;
struct udp_pcb *upcb_server;

void udp_client_send(uint8_t *data, uint16_t len);

/* Private functions ---------------------------------------------------------*/
void udp_server_receive_callback(void *arg, struct udp_pcb *upcb, struct pbuf *p, const ip_addr_t *addr, u16_t port)
{
	uint16_t data_len;
	if (upcb_client->flags & UDP_FLAGS_CONNECTED)
	{
		/* free the UDP connection, so we can accept new clients */
		udp_disconnect(upcb_client);
	}

	if (!(upcb_client->flags & UDP_FLAGS_CONNECTED))
	{
		data_len = mb_rsp_connect((uint8_t *)(p->payload), (uint16_t)(p->len), udp_send_buf);
		if (data_len != 0)
		{
			udp_send_active_flag = 1;
			/* Connect to the remote client */
			udp_connect(upcb_client, addr, port);
			udp_client_send(udp_send_buf, data_len);
			LCD_UsrLog ("UdpConnectComplete\n");
			user_time_start();
		}
	}

	/* Free receive pbuf */
	pbuf_free(p);
}
/**
  * @brief  Connect to UDP echo server
  * @param  None
  * @retval None
  */
void udp_echoclient_connect(void)
{
  ip_addr_t DestIPaddr;
  err_t err;
  
  /* Create a new UDP control block  */
  upcb_client = udp_new();
  
  if (upcb_client != NULL)
  {
    /*assign destination IP address */
    IP4_ADDR( &DestIPaddr, DEST_IP_ADDR0, DEST_IP_ADDR1, DEST_IP_ADDR2, DEST_IP_ADDR3 );
    
	udp_bind(upcb_client,IP_ADDR_ANY,UDP_CLIENT_PORT);
	  
    /* configure destination IP address and port */
    err = udp_connect(upcb_client, &DestIPaddr, UDP_REMOTE_PORT); 
	  
    if (err == ERR_OK)
    {
      /* Set a receive callback for the upcb */
      udp_recv(upcb_client, udp_receive_callback, NULL);  
    }
  }
  
  upcb_server = udp_new();
  if (upcb_server != NULL)
  {
	/* Bind the upcb to the UDP_PORT port */
    /* Using IP_ADDR_ANY allow the upcb to be used by any local interface */
	err = udp_bind(upcb_server,IP_ADDR_ANY,UDP_SERVER_PORT);
    
    if (err == ERR_OK)
    {
      /* Set a receive callback for the upcb */
      udp_recv(upcb_server, udp_server_receive_callback, NULL);  
    }
  }
}

/**
  * @brief This function is called when an UDP datagrm has been received on the port UDP_PORT.
  * @param arg user supplied argument (udp_pcb.recv_arg)
  * @param pcb the udp_pcb which received data
  * @param p the packet buffer that was received
  * @param addr the remote IP address from which the packet was received
  * @param port the remote port from which the packet was received
  * @retval None
  */
void udp_echoclient_send(void)
{
	uint16_t data_len;
	
	if (get_timer2_isr_flag() == 1)
	{
		clr_timer2_isr_flag();
		modbus_bus485_task();
	}
	
	if (get_ltc_frame_update_event() == 1)
	{
		clr_ltc_frame_update_event();
#ifdef USE_DHCP
		if ((!(upcb_client->flags & UDP_FLAGS_CONNECTED)) ||
			((DHCP_state != DHCP_ADDRESS_ASSIGNED) &&
			 (DHCP_state != DHCP_TIMEOUT)))
#else
		if (!(upcb_client->flags & UDP_FLAGS_CONNECTED))
#endif
		{
			return;
		}
		data_len = send_ltc(udp_send_buf);
		udp_client_send(udp_send_buf, data_len);
		HAL_GPIO_TogglePin(OUTPUT_LED1_GPIO_Port, OUTPUT_LED1_Pin);
	}
	
	/* Fine DHCP periodic process every 500ms */
	if (HAL_GetTick() - UDPSendTimer >= UDP_SEND_TIMER_MSECS)
	{
		modbus_udp_task();
		UDPSendTimer =  HAL_GetTick(); 
		if (0 == udp_send_active_flag)
		{
			return;
		}
#ifdef USE_DHCP
		if ((!(upcb_client->flags & UDP_FLAGS_CONNECTED)) ||
			((DHCP_state != DHCP_ADDRESS_ASSIGNED) &&
			 (DHCP_state != DHCP_TIMEOUT)))
#else
		if (!(upcb_client->flags & UDP_FLAGS_CONNECTED))
#endif
		{
			return;
		}
	}
	else
	{
		return;
	}
}

void udp_client_send(uint8_t *data, uint16_t len)
{
  struct pbuf *p;
  
  /* allocate pbuf from pool*/
  p = pbuf_alloc(PBUF_TRANSPORT, len, PBUF_POOL);
  
  if (p != NULL)
  {
    /* copy data to pbuf */
    pbuf_take(p, data, len);
    
    /* send udp data */
    udp_send(upcb_client, p); 

    /* free pbuf */
    pbuf_free(p);
  }
}

/**
  * @brief This function is called when an UDP datagrm has been received on the port UDP_PORT.
  * @param arg user supplied argument (udp_pcb.recv_arg)
  * @param pcb the udp_pcb which received data
  * @param p the packet buffer that was received
  * @param addr the remote IP address from which the packet was received
  * @param port the remote port from which the packet was received
  * @retval None
  */
void udp_receive_callback(void *arg, struct udp_pcb *upcb, struct pbuf *p, const ip_addr_t *addr, u16_t port)
{
	uint16_t len;

	len = mb_rsp((uint8_t *)(p->payload), (uint16_t)(p->len), udp_send_buf);
	if (len != 0)
	{
		udp_client_send(udp_send_buf, len);
	}
	/* Free receive pbuf */
	pbuf_free(p);
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
