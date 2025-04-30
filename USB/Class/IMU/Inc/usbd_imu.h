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

/** @addtogroup STM32_USB_DEVICE_LIBRARY
  * @{
  */

/** @defgroup USBD_IMU
  * @brief Header file for IMU bulk data endpoint implementation
  * @{
  */

/** @defgroup USBD_IMU_Exported_Defines
  * @{
  */
#ifndef IMU_EPIN_ADDR
#define IMU_EPIN_ADDR                  0x81U  /* EP1 IN (adjust based on your endpoint allocation) */
#endif /* IMU_EPIN_ADDR */

/* Packet sizes - adjust based on your needs */
#define IMU_FS_MAX_PACKET_SIZE         64U    /* Full-speed USB */
#define IMU_HS_MAX_PACKET_SIZE         512U   /* High-speed USB */

/* Bulk endpoints don't use intervals */
#define IMU_BINTERVAL                  0U

/* Descriptor sizes */
#define IMU_INTERFACE_DESC_SIZE        9U
#define IMU_ENDPOINT_DESC_SIZE         7U

/**
  * @}
  */

/** @defgroup USBD_IMU_Exported_Types
  * @{
  */
typedef enum
{
  IMU_IDLE = 0,
  IMU_BUSY,
} IMU_StateTypeDef;

typedef struct
{
  IMU_StateTypeDef state;
  uint8_t ep_in;
  uint16_t ep_in_size;
} USBD_IMU_HandleTypeDef;

/**
  * @}
  */

/** @defgroup USBD_IMU_Exported_Macros
  * @{
  */

/**
  * @}
  */

/** @defgroup USBD_IMU_Exported_Variables
  * @{
  */

extern USBD_ClassTypeDef USBD_IMU;
#define USBD_IMU_CLASS &USBD_IMU

/**
  * @}
  */

/** @defgroup USBD_IMU_Exported_Functions
  * @{
  */

#ifdef USE_USBD_COMPOSITE
uint8_t USBD_IMU_SendData(USBD_HandleTypeDef *pdev, uint8_t *data, uint16_t len, uint8_t ClassId);
#else
uint8_t USBD_IMU_SendData(USBD_HandleTypeDef *pdev, uint8_t *data, uint16_t len);
#endif /* USE_USBD_COMPOSITE */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* __USB_IMU_H */

/**
  * @}
  */

/**
  * @}
  */
