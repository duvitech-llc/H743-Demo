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
  NULL,
  NULL,
  NULL,
  NULL,
#endif /* USE_USBD_COMPOSITE */
};

/* Private functions */
static uint8_t USBD_Histo_Init(USBD_HandleTypeDef *pdev, uint8_t cfgidx)
{
  UNUSED(cfgidx);
  USBD_Histo_HandleTypeDef *hhisto;

  hhisto = USBD_malloc(sizeof(USBD_Histo_HandleTypeDef));
  if (hhisto == NULL) {
    return (uint8_t)USBD_EMEM;
  }

  pdev->pClassData = (void *)hhisto;
  hhisto->state = HISTO_IDLE;
  hhisto->buffer_idx = 0;
  hhisto->data_ready = 0;

  /* Open endpoints */
  USBD_LL_OpenEP(pdev, HISTO_EP_IN_1, USBD_EP_TYPE_BULK, HISTO_DATA_PACKET_SIZE);
  USBD_LL_OpenEP(pdev, HISTO_EP_IN_2, USBD_EP_TYPE_BULK, HISTO_DATA_PACKET_SIZE);
  USBD_LL_OpenEP(pdev, HISTO_EP_IN_3, USBD_EP_TYPE_BULK, HISTO_DATA_PACKET_SIZE);
  USBD_LL_OpenEP(pdev, HISTO_EP_IN_4, USBD_EP_TYPE_BULK, HISTO_DATA_PACKET_SIZE);

  return (uint8_t)USBD_OK;
}

static uint8_t USBD_Histo_DeInit(USBD_HandleTypeDef *pdev, uint8_t cfgidx)
{
  UNUSED(cfgidx);

  /* Close endpoints */
  USBD_LL_CloseEP(pdev, HISTO_EP_IN_1);
  USBD_LL_CloseEP(pdev, HISTO_EP_IN_2);
  USBD_LL_CloseEP(pdev, HISTO_EP_IN_3);
  USBD_LL_CloseEP(pdev, HISTO_EP_IN_4);

  /* Free memory */
  if (pdev->pClassData != NULL) {
    USBD_free(pdev->pClassData);
    pdev->pClassData = NULL;
  }

  return (uint8_t)USBD_OK;
}

static uint8_t USBD_Histo_Setup(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req)
{
  /* Handle only standard requests */
  switch (req->bmRequest & USB_REQ_TYPE_MASK) {
    case USB_REQ_TYPE_STANDARD:
      switch (req->bRequest) {
        case USB_REQ_GET_STATUS:
        case USB_REQ_CLEAR_FEATURE:
          return (uint8_t)USBD_OK;

        default:
          USBD_CtlError(pdev, req);
          return (uint8_t)USBD_FAIL;
      }
      break;

    default:
      USBD_CtlError(pdev, req);
      return (uint8_t)USBD_FAIL;
  }
}

static uint8_t USBD_Histo_DataIn(USBD_HandleTypeDef *pdev, uint8_t epnum)
{
  USBD_Histo_HandleTypeDef *hhisto = (USBD_Histo_HandleTypeDef *)pdev->pClassData;

  if (hhisto == NULL) {
    return (uint8_t)USBD_FAIL;
  }

  /* Mark transfer complete */
  hhisto->state = HISTO_IDLE;

  return (uint8_t)USBD_OK;
}

uint8_t USBD_HISTO_SendData(USBD_HandleTypeDef *pdev, uint8_t *data, uint16_t len, uint8_t ep_idx)
{
  USBD_Histo_HandleTypeDef *hhisto = (USBD_Histo_HandleTypeDef *)pdev->pClassData;
  uint8_t ep_addr;

  if (hhisto == NULL || pdev->dev_state != USBD_STATE_CONFIGURED) {
    return (uint8_t)USBD_FAIL;
  }

  /* Select endpoint based on index */
  switch (ep_idx) {
    case 0: ep_addr = HISTO_EP_IN_1; break;
    case 1: ep_addr = HISTO_EP_IN_2; break;
    case 2: ep_addr = HISTO_EP_IN_3; break;
    case 3: ep_addr = HISTO_EP_IN_4; break;
    default: return (uint8_t)USBD_FAIL;
  }

  if (hhisto->state == HISTO_IDLE) {
    hhisto->state = HISTO_BUSY;
    if (USBD_LL_Transmit(pdev, ep_addr, data, len) != USBD_OK) {
      hhisto->state = HISTO_IDLE;
      return (uint8_t)USBD_FAIL;
    }
  }

  return (uint8_t)USBD_OK;
}

uint8_t USBD_HISTO_RegisterInterface(USBD_HandleTypeDef *pdev, uint8_t *buffer)
{
  UNUSED(pdev);
  UNUSED(buffer);
  return (uint8_t)USBD_OK;
}
