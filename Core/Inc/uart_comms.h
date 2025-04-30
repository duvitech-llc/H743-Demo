/*
 * uart_comms.h
 *
 *  Created on: Sep 30, 2024
 *      Author: GeorgeVigelette
 */

#ifndef INC_UART_COMMS_H_
#define INC_UART_COMMS_H_

#include "main.h"  // This should contain your HAL includes and other basic includes.
#include <stdio.h>
#include <stdbool.h>

#define COMMAND_MAX_SIZE 1024U

void comms_host_check_received(void);
void comms_handle_RxCpltCallback(UART_HandleTypeDef *huart, uint16_t Size);
void comms_handle_TxCallback(UART_HandleTypeDef *huart);
void CDC_handle_TxCpltCallback();
void comms_interface_send(UartPacket* pResp);
void comms_host_start(void);

#endif /* INC_UART_COMMS_H_ */
