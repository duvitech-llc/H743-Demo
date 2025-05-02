/**
  ******************************************************************************
  * @file    usbd_imu.h
  * @author  Your Name
  * @brief   Header file for the USB IMU bulk endpoint implementation
  ******************************************************************************
  * @attention
  *
  * Copyright (c) [Year] STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USB_IMU_H
#define __USB_IMU_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "usbd_ioreq.h"

#ifndef IMU_IN_EP
#define IMU_IN_EP                  0x81U  /* EP1 IN (adjust based on your endpoint allocation) */
#endif /* IMU_IN_EP */

/* Packet sizes - adjust based on your needs */
#define IMU_FS_MAX_PACKET_SIZE         64U    /* Full-speed USB */
#define IMU_HS_MAX_PACKET_SIZE         512U   /* High-speed USB */

/* Bulk endpoints don't use intervals */
#define IMU_BINTERVAL                  0U

/* Descriptor sizes */
#define IMU_INTERFACE_DESC_SIZE        9U
#define IMU_ENDPOINT_DESC_SIZE         7U

extern USBD_ClassTypeDef USBD_IMU;
#define USBD_IMU_CLASS &USBD_IMU

#ifdef __cplusplus
}
#endif

#endif /* __USB_IMU_H */

