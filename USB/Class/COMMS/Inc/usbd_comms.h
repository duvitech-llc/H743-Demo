/**
  ******************************************************************************
  * @file    usbd_comms.h
  * @brief   Header file for USB comms
  ******************************************************************************
  */
#ifndef __USB_COMMS_H
#define __USB_COMMS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "usbd_ioreq.h"


#ifndef COMMS_IN_EP
#define COMMS_IN_EP                                   0x81U  /* EP1 for data IN */
#endif /* HISTO_IN_EP */

#ifndef COMMS_OUT_EP
#define COMMS_OUT_EP                        0x01U  /* EP1 for data OUT */
#endif

#define COMMS_FS_MAX_PACKET_SIZE         64U    /* Full-speed USB */
#define COMMS_HS_MAX_PACKET_SIZE         512U   /* High-speed USB */
#define USB_COMMS_MAX_SIZE 				 1024U
extern USBD_ClassTypeDef USBD_COMMS;
#define USBD_COMMS_CLASS &USBD_COMMS

uint8_t  USBD_COMMS_SetTxBuffer(USBD_HandleTypeDef *pdev, uint8_t  *pbuff, uint16_t length);
void COMMS_Idle_Timer_Handler();
uint8_t COMMS_Transmit(uint8_t* Buf, uint16_t Len);
void COMMS_FlushRxBuffer();
void COMMS_ReceiveToIdle(uint8_t* Buf, uint16_t max_size);
uint8_t USBD_COMMS_RegisterRxCallback(void (*cb)(uint8_t *buf, uint16_t len));

#ifdef __cplusplus
}
#endif

#endif /* __USB_COMMS_H */
