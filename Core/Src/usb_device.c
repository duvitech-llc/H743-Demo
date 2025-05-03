/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : usb_device.c
  * @version        : v1.0_Cube
  * @brief          : This file implements the USB Device
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

/* Includes ------------------------------------------------------------------*/

#include "usb_device.h"
#include "usbd_core.h"
#include "usbd_desc.h"
#include "usbd_cdc.h"
#include "usbd_histo.h"
#include "usbd_cdc_if.h"
#include "usbd_composite_builder.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

uint8_t CDC_EpAdd_Inst[3] = {CDC_IN_EP, CDC_OUT_EP, CDC_CMD_EP}; 	/* CDC Endpoint Addresses array */
uint8_t IMU_EpAdd_Inst[1] = { IMU_IN_EP }; 	/* IMU Endpoint */
uint8_t HISTO_EpAdd_Inst[1] = { HISTO_IN_EP }; 	/* HISTO Endpoint */

uint8_t CDC_InstID = 0, HISTO_InstID = 0, IMU_InstID = 0;

/* USER CODE END PV */

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* USB Device Core handle declaration. */
USBD_HandleTypeDef hUsbDeviceFS;

/*
 * -- Insert your variables declaration here --
 */
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/*
 * -- Insert your external function declaration here --
 */
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/**
  * Init USB device Library, add supported class and start the library
  * @retval None
  */
void MX_USB_DEVICE_Init(void)
{
  /* USER CODE BEGIN USB_DEVICE_Init_PreTreatment */

  /* USER CODE END USB_DEVICE_Init_PreTreatment */

  /* Init Device Library, add supported class and start the library. */
  if (USBD_Init(&hUsbDeviceFS, &FS_Desc, DEVICE_FS) != USBD_OK)
  {
    Error_Handler();
  }

  /* Store the CDC Class */
  CDC_InstID = hUsbDeviceFS.classId;

  /* Register CDC Class First Instance */
  if(USBD_RegisterClassComposite(&hUsbDeviceFS, USBD_CDC_CLASS, CLASS_TYPE_CDC, CDC_EpAdd_Inst) != USBD_OK)
  {
	Error_Handler();
  }

#if 1
  /* Store HISTO Instance Class ID */
  HISTO_InstID = hUsbDeviceFS.classId;

  /* Register the HISTO Class */
  if(USBD_RegisterClassComposite(&hUsbDeviceFS, USBD_HISTO_CLASS, CLASS_TYPE_HISTO, HISTO_EpAdd_Inst) != USBD_OK)
  {
	Error_Handler();
  }

  /* Store HISTO Instance Class ID */
  IMU_InstID = hUsbDeviceFS.classId;

  /* Register the HISTO Class */
  if(USBD_RegisterClassComposite(&hUsbDeviceFS, USBD_IMU_CLASS, CLASS_TYPE_IMU, IMU_EpAdd_Inst) != USBD_OK)
  {
	Error_Handler();
  }
#endif

  /* Add CDC Interface Class */
  if (USBD_CMPSIT_SetClassID(&hUsbDeviceFS, CLASS_TYPE_CDC, CDC_InstID) != 0xFF)
  {
	USBD_CDC_RegisterInterface(&hUsbDeviceFS, &USBD_Interface_fops);
  }

  if (USBD_Start(&hUsbDeviceFS) != USBD_OK)
  {
    Error_Handler();
  }

  /* USER CODE BEGIN USB_DEVICE_Init_PostTreatment */
  HAL_PWREx_EnableUSBVoltageDetector();

  /* USER CODE END USB_DEVICE_Init_PostTreatment */
}

/**
  * @}
  */

/**
  * @}
  */

