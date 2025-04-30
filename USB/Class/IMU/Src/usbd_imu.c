/**
  ******************************************************************************
  * @file    usbd_imu.c
  * @author  Your Name
  * @brief   This file provides the IMU bulk endpoint core functions.
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

/* Includes ------------------------------------------------------------------*/
#include "usbd_imu.h"
#include "usbd_ctlreq.h"

/** @addtogroup STM32_USB_DEVICE_LIBRARY
  * @{
  */

/** @defgroup USBD_IMU
  * @brief IMU bulk endpoint module
  * @{
  */

/** @defgroup USBD_IMU_Private_TypesDefinitions
  * @{
  */
/**
  * @}
  */

/** @defgroup USBD_IMU_Private_Defines
  * @{
  */
#define IMU_CONFIG_DESC_SIZ        (9 + 9 + 7)  // Interface + Endpoint descriptors
/**
  * @}
  */

/** @defgroup USBD_IMU_Private_Macros
  * @{
  */
/**
  * @}
  */

/** @defgroup USBD_IMU_Private_FunctionPrototypes
  * @{
  */
static uint8_t USBD_IMU_Init(USBD_HandleTypeDef *pdev, uint8_t cfgidx);
static uint8_t USBD_IMU_DeInit(USBD_HandleTypeDef *pdev, uint8_t cfgidx);
static uint8_t USBD_IMU_Setup(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req);
static uint8_t USBD_IMU_DataIn(USBD_HandleTypeDef *pdev, uint8_t epnum);
#ifndef USE_USBD_COMPOSITE
static uint8_t *USBD_IMU_GetFSCfgDesc(uint16_t *length);
static uint8_t *USBD_IMU_GetHSCfgDesc(uint16_t *length);
static uint8_t *USBD_IMU_GetOtherSpeedCfgDesc(uint16_t *length);
static uint8_t *USBD_IMU_GetDeviceQualifierDesc(uint16_t *length);
#endif /* USE_USBD_COMPOSITE */
/**
  * @}
  */

/** @defgroup USBD_IMU_Private_Variables
  * @{
  */

USBD_ClassTypeDef USBD_IMU =
{
  USBD_IMU_Init,
  USBD_IMU_DeInit,
  USBD_IMU_Setup,
  NULL,                 /* EP0_TxSent */
  NULL,                 /* EP0_RxReady */
  USBD_IMU_DataIn,      /* DataIn */
  NULL,                 /* DataOut */
  NULL,                 /* SOF */
  NULL,
  NULL,
#ifdef USE_USBD_COMPOSITE
  NULL,
  NULL,
  NULL,
  NULL,
#else
  USBD_IMU_GetHSCfgDesc,
  USBD_IMU_GetFSCfgDesc,
  USBD_IMU_GetOtherSpeedCfgDesc,
  USBD_IMU_GetDeviceQualifierDesc,
#endif /* USE_USBD_COMPOSITE */
};

#ifndef USE_USBD_COMPOSITE
/* USB IMU device FS Configuration Descriptor */
__ALIGN_BEGIN static uint8_t USBD_IMU_CfgDesc[IMU_CONFIG_DESC_SIZ] __ALIGN_END =
{
  /* Interface Descriptor */
  0x09,                           /* bLength: Interface Descriptor size */
  USB_DESC_TYPE_INTERFACE,        /* bDescriptorType: Interface */
  0x00,                           /* bInterfaceNumber: Number of Interface */
  0x00,                           /* bAlternateSetting: Alternate setting */
  0x01,                           /* bNumEndpoints: 1 endpoint used */
  0xFF,                           /* bInterfaceClass: Vendor Specific */
  0x00,                           /* bInterfaceSubClass */
  0x00,                           /* bInterfaceProtocol */
  0x00,                           /* iInterface: Index of string descriptor */

  /* Endpoint Descriptor */
  0x07,                           /* bLength: Endpoint Descriptor size */
  USB_DESC_TYPE_ENDPOINT,         /* bDescriptorType: Endpoint */
  IMU_IN_EP,                  /* bEndpointAddress: (IN) */
  0x02,                           /* bmAttributes: Bulk */
  IMU_FS_MAX_PACKET_SIZE,         /* wMaxPacketSize: */
  0x00,
  0x00                            /* bInterval: ignored for bulk */
};
#endif /* USE_USBD_COMPOSITE */

static uint8_t IMUInEpAdd = IMU_IN_EP;

/**
  * @}
  */

/** @defgroup USBD_IMU_Private_Functions
  * @{
  */

/**
  * @brief  USBD_IMU_Init
  *         Initialize the IMU interface
  */
static uint8_t USBD_IMU_Init(USBD_HandleTypeDef *pdev, uint8_t cfgidx)
{
  UNUSED(cfgidx);
  USBD_IMU_HandleTypeDef *himu;

  himu = (USBD_IMU_HandleTypeDef *)USBD_malloc(sizeof(USBD_IMU_HandleTypeDef));

  if (himu == NULL)
  {
    pdev->pClassDataCmsit[pdev->classId] = NULL;
    return (uint8_t)USBD_EMEM;
  }

  pdev->pClassDataCmsit[pdev->classId] = (void *)himu;
  pdev->pClassData = pdev->pClassDataCmsit[pdev->classId];

#ifdef USE_USBD_COMPOSITE
  /* Get the Endpoints addresses allocated for this class instance */
  IMUInEpAdd = USBD_CoreGetEPAdd(pdev, USBD_EP_IN, USBD_EP_TYPE_BULK, (uint8_t)pdev->classId);
#endif /* USE_USBD_COMPOSITE */

  /* Open EP IN */
  (void)USBD_LL_OpenEP(pdev, IMUInEpAdd, USBD_EP_TYPE_BULK,
                      (pdev->dev_speed == USBD_SPEED_HIGH) ?
                      IMU_HS_MAX_PACKET_SIZE : IMU_FS_MAX_PACKET_SIZE);

  pdev->ep_in[IMUInEpAdd & 0xFU].is_used = 1U;
  himu->state = IMU_IDLE;

  return (uint8_t)USBD_OK;
}

/**
  * @brief  USBD_IMU_DeInit
  *         DeInitialize the IMU layer
  */
static uint8_t USBD_IMU_DeInit(USBD_HandleTypeDef *pdev, uint8_t cfgidx)
{
  UNUSED(cfgidx);

#ifdef USE_USBD_COMPOSITE
  /* Get the Endpoints addresses allocated for this class instance */
  IMUInEpAdd = USBD_CoreGetEPAdd(pdev, USBD_EP_IN, USBD_EP_TYPE_BULK, (uint8_t)pdev->classId);
#endif /* USE_USBD_COMPOSITE */

  /* Close IMU EP */
  (void)USBD_LL_CloseEP(pdev, IMUInEpAdd);
  pdev->ep_in[IMUInEpAdd & 0xFU].is_used = 0U;

  /* Free allocated memory */
  if (pdev->pClassDataCmsit[pdev->classId] != NULL)
  {
    (void)USBD_free(pdev->pClassDataCmsit[pdev->classId]);
    pdev->pClassDataCmsit[pdev->classId] = NULL;
  }

  return (uint8_t)USBD_OK;
}

/**
  * @brief  USBD_IMU_Setup
  *         Handle the IMU specific requests
  */
static uint8_t USBD_IMU_Setup(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req)
{
  USBD_StatusTypeDef ret = USBD_OK;

  switch (req->bmRequest & USB_REQ_TYPE_MASK)
  {
    case USB_REQ_TYPE_STANDARD:
      switch (req->bRequest)
      {
        case USB_REQ_GET_STATUS:
          /* No action needed for bulk endpoints */
          break;

        case USB_REQ_CLEAR_FEATURE:
          /* No action needed for bulk endpoints */
          break;

        default:
          USBD_CtlError(pdev, req);
          ret = USBD_FAIL;
          break;
      }
      break;

    default:
      USBD_CtlError(pdev, req);
      ret = USBD_FAIL;
      break;
  }

  return (uint8_t)ret;
}

/**
  * @brief  USBD_IMU_SendData
  *         Send IMU data
  */
#ifdef USE_USBD_COMPOSITE
uint8_t USBD_IMU_SendData(USBD_HandleTypeDef *pdev, uint8_t *data, uint16_t len, uint8_t ClassId)
{
  USBD_IMU_HandleTypeDef *himu = (USBD_IMU_HandleTypeDef *)pdev->pClassDataCmsit[ClassId];
#else
uint8_t USBD_IMU_SendData(USBD_HandleTypeDef *pdev, uint8_t *data, uint16_t len)
{
  USBD_IMU_HandleTypeDef *himu = (USBD_IMU_HandleTypeDef *)pdev->pClassDataCmsit[pdev->classId];
#endif /* USE_USBD_COMPOSITE */

  if (himu == NULL)
  {
    return (uint8_t)USBD_FAIL;
  }

#ifdef USE_USBD_COMPOSITE
  /* Get the Endpoints addresses allocated for this class instance */
  IMUInEpAdd = USBD_CoreGetEPAdd(pdev, USBD_EP_IN, USBD_EP_TYPE_BULK, ClassId);
#endif /* USE_USBD_COMPOSITE */

  if (pdev->dev_state == USBD_STATE_CONFIGURED)
  {
    if (himu->state == IMU_IDLE)
    {
      himu->state = IMU_BUSY;
      (void)USBD_LL_Transmit(pdev, IMUInEpAdd, data, len);
    }
  }

  return (uint8_t)USBD_OK;
}

/**
  * @brief  USBD_IMU_DataIn
  *         Handle data IN Stage
  */
static uint8_t USBD_IMU_DataIn(USBD_HandleTypeDef *pdev, uint8_t epnum)
{
  UNUSED(epnum);

  ((USBD_IMU_HandleTypeDef *)pdev->pClassDataCmsit[pdev->classId])->state = IMU_IDLE;

  return (uint8_t)USBD_OK;
}

#ifndef USE_USBD_COMPOSITE
/**
  * @brief  USBD_IMU_GetFSCfgDesc
  *         Return FS configuration descriptor
  */
static uint8_t *USBD_IMU_GetFSCfgDesc(uint16_t *length)
{
  *length = (uint16_t)sizeof(USBD_IMU_CfgDesc);
  return USBD_IMU_CfgDesc;
}

/**
  * @brief  USBD_IMU_GetHSCfgDesc
  *         Return HS configuration descriptor
  */
static uint8_t *USBD_IMU_GetHSCfgDesc(uint16_t *length)
{
  *length = (uint16_t)sizeof(USBD_IMU_CfgDesc);
  return USBD_IMU_CfgDesc;
}

/**
  * @brief  USBD_IMU_GetOtherSpeedCfgDesc
  *         Return other speed configuration descriptor
  */
static uint8_t *USBD_IMU_GetOtherSpeedCfgDesc(uint16_t *length)
{
  *length = (uint16_t)sizeof(USBD_IMU_CfgDesc);
  return USBD_IMU_CfgDesc;
}

/**
  * @brief  USBD_IMU_GetDeviceQualifierDesc
  *         Return Device Qualifier descriptor
  */
static uint8_t *USBD_IMU_GetDeviceQualifierDesc(uint16_t *length)
{
  *length = 0;
  return NULL;
}
#endif /* USE_USBD_COMPOSITE */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */
