/**
  ******************************************************************************
  * @file    usbd_histo.c
  * @brief   Histogram data streaming implementation
  ******************************************************************************
  */
#include "usbd_histo.h"
#include "usbd_ctlreq.h"
#include "usbd_desc.h"

/* Private typedef */
enum {
  HISTO_IDLE = 0,
  HISTO_BUSY
};

/* Private variables */
static uint8_t USBD_Histo_Init(USBD_HandleTypeDef *pdev, uint8_t cfgidx);
static uint8_t USBD_Histo_DeInit(USBD_HandleTypeDef *pdev, uint8_t cfgidx);
static uint8_t USBD_Histo_Setup(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req);
static uint8_t USBD_Histo_DataIn(USBD_HandleTypeDef *pdev, uint8_t epnum);
#ifndef USE_USBD_COMPOSITE
static uint8_t *USBD_Histo_GetFSCfgDesc(uint16_t *length);
static uint8_t *USBD_Histo_GetHSCfgDesc(uint16_t *length);
static uint8_t *USBD_Histo_GetOtherSpeedCfgDesc(uint16_t *length);
uint8_t *USBD_Histo_GetDeviceQualifierDescriptor(uint16_t *length);
#endif /* USE_USBD_COMPOSITE  */

USBD_ClassTypeDef USBD_HISTO = {
  USBD_Histo_Init,
  USBD_Histo_DeInit,
  USBD_Histo_Setup,
  NULL,                 /* EP0_TxSent */
  NULL,                 /* EP0_RxReady */
  USBD_Histo_DataIn,    /* DataIn */
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
  USBD_Histo_GetHSCfgDesc,
  USBD_Histo_GetFSCfgDesc,
  USBD_Histo_GetOtherSpeedCfgDesc,
  USBD_Histo_GetDeviceQualifierDescriptor,
#endif /* USE_USBD_COMPOSITE  */
};

#ifndef USE_USBD_COMPOSITE
/* USB Standard Device Descriptor */
__ALIGN_BEGIN static uint8_t USBD_Histo_GetDeviceQualifierDescriptor[USB_LEN_DEV_QUALIFIER_DESC] __ALIGN_END =
{
  USB_LEN_DEV_QUALIFIER_DESC,
  USB_DESC_TYPE_DEVICE_QUALIFIER,
  0x00,
  0x02,
  0x00,
  0x00,
  0x00,
  0x40,
  0x01,
  0x00,
};
#endif /* USE_USBD_COMPOSITE  */

static uint8_t HISTOInEpAdd = HISTO_IN_EP;

/* Private functions */
static uint8_t USBD_Histo_Init(USBD_HandleTypeDef *pdev, uint8_t cfgidx)
{
  UNUSED(cfgidx);

  #ifdef USE_USBD_COMPOSITE
    /* Get the Endpoints addresses allocated for this class instance */
    HISTOInEpAdd  = USBD_CoreGetEPAdd(pdev, USBD_EP_IN, USBD_EP_TYPE_BULK, (uint8_t)pdev->classId);
  #endif /* USE_USBD_COMPOSITE */
  
    if (pdev->dev_speed == USBD_SPEED_HIGH)
    {
      /* Open EP IN */
      (void)USBD_LL_OpenEP(pdev, HISTOInEpAdd, USBD_EP_TYPE_BULK, HISTO_DATA_PACKET_SIZE);
      pdev->ep_in[HISTOInEpAdd & 0xFU].bInterval = 0;
    }
    else
    {
    /* Open EP IN */
    (void)USBD_LL_OpenEP(pdev, HISTOInEpAdd, USBD_EP_TYPE_BULK, HISTO_DATA_PACKET_SIZE);
      pdev->ep_in[HISTOInEpAdd & 0xFU].bInterval = 0;
    }
  
    pdev->ep_in[HISTOInEpAdd & 0xFU].is_used = 1U;
    return (uint8_t)USBD_OK;
}

static uint8_t USBD_Histo_DeInit(USBD_HandleTypeDef *pdev, uint8_t cfgidx)
{
  UNUSED(cfgidx);

#ifdef USE_USBD_COMPOSITE
  /* Get the Endpoints addresses allocated for this CDC class instance */
  HISTOInEpAdd  = USBD_CoreGetEPAdd(pdev, USBD_EP_IN, USBD_EP_TYPE_BULK, (uint8_t)pdev->classId);
#endif /* USE_USBD_COMPOSITE */

  /* Close EP IN */
  (void)USBD_LL_CloseEP(pdev, HISTOInEpAdd);
  pdev->ep_in[HISTOInEpAdd & 0xFU].is_used = 0U;

#ifdef USE_USBD_COMPOSITE
  if (pdev->pClassDataCmsit[pdev->classId] != NULL)
  {
    // ((USBD_HISTO_ItfTypeDef *)pdev->pUserData[pdev->classId])->DeInit();
    (void)USBD_free(pdev->pClassDataCmsit[pdev->classId]);
    pdev->pClassDataCmsit[pdev->classId] = NULL;
    pdev->pClassData = NULL;
  }

#else
  /* Free memory */
  if (pdev->pClassData != NULL) {
    USBD_free(pdev->pClassData);
    pdev->pClassData = NULL;
  }
#endif

  return (uint8_t)USBD_OK;
}

static uint8_t USBD_Histo_Setup(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req)
{
  // Ignore everything, don't stall
  return (uint8_t)USBD_OK;
}

static uint8_t USBD_Histo_DataIn(USBD_HandleTypeDef *pdev, uint8_t epnum)
{
  return (uint8_t)USBD_OK;
}

uint8_t USBD_HISTO_SendData(USBD_HandleTypeDef *pdev, uint8_t *data, uint16_t len, uint8_t ep_idx)
{
  return (uint8_t)USBD_OK;
}

uint8_t USBD_HISTO_RegisterInterface(USBD_HandleTypeDef *pdev, uint8_t *buffer)
{
  UNUSED(pdev);
  UNUSED(buffer);
  return (uint8_t)USBD_OK;
}
