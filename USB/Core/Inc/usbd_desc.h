/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : usbd_desc.c
  * @version        : v1.0_Cube
  * @brief          : Header for usbd_conf.c file.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USBD_DESC__C__
#define __USBD_DESC__C__

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "usbd_def.h"

/* USER CODE BEGIN INCLUDE */

/* USER CODE END INCLUDE */

/** @addtogroup STM32_USB_OTG_DEVICE_LIBRARY
  * @{
  */

/** @defgroup USBD_DESC USBD_DESC
  * @brief Usb device descriptors module.
  * @{
  */

/** @defgroup USBD_DESC_Exported_Constants USBD_DESC_Exported_Constants
  * @brief Constants.
  * @{
  */
#define         DEVICE_ID1          (UID_BASE)
#define         DEVICE_ID2          (UID_BASE + 0x4)
#define         DEVICE_ID3          (UID_BASE + 0x8)

/* USER CODE BEGIN EXPORTED_CONSTANTS */

/* USER CODE END EXPORTED_CONSTANTS */

/**
  * @}
  */

/** @defgroup USBD_DESC_Exported_Defines USBD_DESC_Exported_Defines
  * @brief Defines.
  * @{
  */

/* USER CODE BEGIN EXPORTED_DEFINES */

/* USER CODE END EXPORTED_DEFINES */

/**
  * @}
  */

/** @defgroup USBD_DESC_Exported_TypesDefinitions USBD_DESC_Exported_TypesDefinitions
  * @brief Types.
  * @{
  */

/* USER CODE BEGIN EXPORTED_TYPES */

/* USER CODE END EXPORTED_TYPES */

/**
  * @}
  */

/** @defgroup USBD_DESC_Exported_Macros USBD_DESC_Exported_Macros
  * @brief Aliases.
  * @{
  */

/* USER CODE BEGIN EXPORTED_MACRO */

/* USER CODE END EXPORTED_MACRO */

/**
  * @}
  */

/** @defgroup USBD_DESC_Exported_Variables USBD_DESC_Exported_Variables
  * @brief Public variables.
  * @{
  */

 /*
  * USB Billboard Class USER string desc Defines Template
  * index should start form 0x10 to avoid using the reserved device string desc indexes
  */
 #if (USBD_CLASS_USER_STRING_DESC == 1)
 #define USBD_BB_IF_STRING_INDEX         0x10U
 #define USBD_BB_URL_STRING_INDEX        0x11U
 #define USBD_BB_ALTMODE0_STRING_INDEX   0x12U
 #define USBD_BB_ALTMODE1_STRING_INDEX   0x13U
 /* Add Specific USER string Desc */
 #define USBD_BB_IF_STR_DESC           (uint8_t *)"STM32 BillBoard Interface"
 #define USBD_BB_URL_STR_DESC          (uint8_t *)"www.st.com"
 #define USBD_BB_ALTMODE0_STR_DESC     (uint8_t *)"STM32 Alternate0 Mode"
 #define USBD_BB_ALTMODE1_STR_DESC     (uint8_t *)"STM32 Alternate1 Mode"
 #endif /* USBD_CLASS_USER_STRING_DESC  */

 #define  USB_SIZ_STRING_SERIAL       0x1AU

 #if (USBD_LPM_ENABLED == 1)
 #define  USB_SIZ_BOS_DESC            0x0CU
 #elif (USBD_CLASS_BOS_ENABLED == 1)
 #define  USB_SIZ_BOS_DESC            0x5DU
 #endif /* USBD_LPM_ENABLED  */

/** Descriptor for the Usb device. */
extern USBD_DescriptorsTypeDef FS_Desc;

/* USER CODE BEGIN EXPORTED_VARIABLES */

/* USER CODE END EXPORTED_VARIABLES */

/**
  * @}
  */

/** @defgroup USBD_DESC_Exported_FunctionsPrototype USBD_DESC_Exported_FunctionsPrototype
  * @brief Public functions declaration.
  * @{
  */

/* USER CODE BEGIN EXPORTED_FUNCTIONS */

/* USER CODE END EXPORTED_FUNCTIONS */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* __USBD_DESC__C__ */

