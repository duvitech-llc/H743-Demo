/**
  ******************************************************************************
  * @file    usbd_comms.c
  * @brief   Commsgram data streaming implementation
  ******************************************************************************
  */
#include "usbd_comms.h"
#include "usbd_ctlreq.h"
#include "usbd_desc.h"

/* Private typedef */

#ifndef MIN
#define MIN(a,b) (((a)<(b))?(a):(b))
#endif

/* Private variables */
static uint8_t USBD_Comms_Init(USBD_HandleTypeDef *pdev, uint8_t cfgidx);
static uint8_t USBD_Comms_DeInit(USBD_HandleTypeDef *pdev, uint8_t cfgidx);
static uint8_t USBD_Comms_Setup(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req);
static uint8_t USBD_Comms_DataIn(USBD_HandleTypeDef *pdev, uint8_t epnum);
static uint8_t USBD_Comms_DataOut(USBD_HandleTypeDef *pdev, uint8_t epnum);
#ifndef USE_USBD_COMPOSITE
static uint8_t *USBD_Comms_GetFSCfgDesc(uint16_t *length);
static uint8_t *USBD_Comms_GetHSCfgDesc(uint16_t *length);
static uint8_t *USBD_Comms_GetOtherSpeedCfgDesc(uint16_t *length);
uint8_t *USBD_Comms_GetDeviceQualifierDescriptor(uint16_t *length);
#endif /* USE_USBD_COMPOSITE  */

USBD_ClassTypeDef USBD_COMMS = {
  USBD_Comms_Init,
  USBD_Comms_DeInit,
  USBD_Comms_Setup,
  NULL,                 /* EP0_TxSent */
  NULL,                 /* EP0_RxReady */
  USBD_Comms_DataIn,    /* DataIn */
  USBD_Comms_DataOut,   /* DataOut */
  NULL,                 /* SOF */
  NULL,
  NULL,
#ifdef USE_USBD_COMPOSITE
  NULL,
  NULL,
  NULL,
  NULL,
#else
  USBD_Comms_GetHSCfgDesc,
  USBD_Comms_GetFSCfgDesc,
  USBD_Comms_GetOtherSpeedCfgDesc,
  USBD_Comms_GetDeviceQualifierDescriptor,
#endif /* USE_USBD_COMPOSITE  */
};

#ifndef USE_USBD_COMPOSITE
/* USB Standard Device Descriptor */
__ALIGN_BEGIN static uint8_t USBD_Comms_GetDeviceQualifierDescriptor[USB_LEN_DEV_QUALIFIER_DESC] __ALIGN_END =
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

extern uint8_t COMMS_InstID;
uint8_t* pTxCommsBuff = 0;
static uint16_t tx_comms_total_len = 0;
static uint16_t tx_comms_ptr = 0;
static __IO uint8_t comms_ep_enabled = 0;
__IO uint8_t comms_ep_data = 0;

static uint8_t COMMSInEpAdd = COMMS_IN_EP;
static uint8_t COMMSOutEpAdd = COMMS_OUT_EP;

static uint8_t *pUserRxBuff = NULL;
static uint8_t *pRxCommsBuff = NULL;
static uint16_t rxIndex = 0;
static uint16_t rxMaxSize = 0;

static void (*CommsRxCallback)(uint8_t *buf, uint16_t len) = NULL;
static uint8_t read_to_idle_enabled = 0;

/* Private functions */
static uint8_t USBD_Comms_Init(USBD_HandleTypeDef *pdev, uint8_t cfgidx)
{
  UNUSED(cfgidx);
  uint32_t packet_size = COMMS_FS_MAX_PACKET_SIZE;
  #ifdef USE_USBD_COMPOSITE
    /* Get the Endpoints addresses allocated for this class instance */
    COMMSInEpAdd  = USBD_CoreGetEPAdd(pdev, USBD_EP_IN, USBD_EP_TYPE_BULK, (uint8_t)pdev->classId);
    COMMSOutEpAdd = USBD_CoreGetEPAdd(pdev, USBD_EP_OUT, USBD_EP_TYPE_BULK, (uint8_t)pdev->classId);
  #endif /* USE_USBD_COMPOSITE */
    printf("COMMS_Init DATA IN EP: 0x%02X ClassID: 0x%02X\r\n", COMMSInEpAdd, (uint8_t)pdev->classId);
    printf("COMMS_Init DATA OUT EP: 0x%02X ClassID: 0x%02X\r\n", COMMSOutEpAdd, (uint8_t)pdev->classId);

    pTxCommsBuff = (uint8_t*)malloc(USB_COMMS_MAX_SIZE);
    if(pTxCommsBuff == NULL){
    	Error_Handler();
    }

    if (pdev->dev_speed == USBD_SPEED_HIGH)
    {
		/* Open EPs */
		(void)USBD_LL_OpenEP(pdev, COMMSInEpAdd, USBD_EP_TYPE_BULK, COMMS_HS_MAX_PACKET_SIZE);
		(void)USBD_LL_OpenEP(pdev, COMMSOutEpAdd, USBD_EP_TYPE_BULK, COMMS_HS_MAX_PACKET_SIZE);
		packet_size = COMMS_HS_MAX_PACKET_SIZE;
    }
    else
    {
		/* Open EPs */
		(void)USBD_LL_OpenEP(pdev, COMMSInEpAdd, USBD_EP_TYPE_BULK, COMMS_FS_MAX_PACKET_SIZE);
		(void)USBD_LL_OpenEP(pdev, COMMSOutEpAdd, USBD_EP_TYPE_BULK, COMMS_FS_MAX_PACKET_SIZE);
		packet_size = COMMS_FS_MAX_PACKET_SIZE;
    }

    comms_ep_enabled = 1;
    pdev->ep_in[COMMSInEpAdd & 0xFU].bInterval = 0;
    pdev->ep_in[COMMSInEpAdd & 0xFU].is_used = 1U;

    pRxCommsBuff = (uint8_t*)malloc(packet_size*2);
    rxIndex = 0;
    (void)USBD_LL_PrepareReceive(pdev, COMMSOutEpAdd, pRxCommsBuff, packet_size);
    return (uint8_t)USBD_OK;
}

extern uint8_t COMMS_InstID;

static uint8_t USBD_Comms_DeInit(USBD_HandleTypeDef *pdev, uint8_t cfgidx)
{
  UNUSED(cfgidx);

#ifdef USE_USBD_COMPOSITE
  /* Get the Endpoints addresses allocated for this CDC class instance */
  COMMSInEpAdd  = USBD_CoreGetEPAdd(pdev, USBD_EP_IN, USBD_EP_TYPE_BULK, (uint8_t)COMMS_InstID);
#endif /* USE_USBD_COMPOSITE */

  /* Close EP IN */
  (void)USBD_LL_CloseEP(pdev, COMMSInEpAdd);
  pdev->ep_in[COMMSInEpAdd & 0xFU].is_used = 0U;
  pdev->ep_in[COMMSInEpAdd & 0xFU].total_length = 0U;
  comms_ep_enabled = 0;

  if(pTxCommsBuff){
	free(pTxCommsBuff);
	pTxCommsBuff = 0;
  }
#ifdef USE_USBD_COMPOSITE
  if (pdev->pClassDataCmsit[pdev->classId] != NULL)
  {
    // ((USBD_COMMS_ItfTypeDef *)pdev->pUserData[pdev->classId])->DeInit();
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

static uint8_t USBD_Comms_Setup(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req)
{
  // Ignore everything, don't stall
  return (uint8_t)USBD_OK;
}

static uint8_t USBD_Comms_DataIn(USBD_HandleTypeDef *pdev, uint8_t epnum)
{
	uint8_t ret = USBD_OK;

#ifdef USE_USBD_COMPOSITE
	  /* Get the Endpoints addresses allocated for this CDC class instance */
	COMMSInEpAdd  = USBD_CoreGetEPAdd(pdev, USBD_EP_IN, USBD_EP_TYPE_BULK, COMMS_InstID);
#endif /* USE_USBD_COMPOSITE */

  if(comms_ep_data==1){
      tx_comms_ptr += (pdev->dev_speed == USBD_SPEED_HIGH)?COMMS_HS_MAX_PACKET_SIZE:COMMS_FS_MAX_PACKET_SIZE;

      if (tx_comms_ptr < tx_comms_total_len)
      {
          uint16_t remaining = tx_comms_total_len - tx_comms_ptr;
          uint16_t pkt_len = MIN((pdev->dev_speed == USBD_SPEED_HIGH)?COMMS_HS_MAX_PACKET_SIZE:COMMS_FS_MAX_PACKET_SIZE, remaining);

          ret =  USBD_LL_Transmit(pdev, COMMSInEpAdd, &pTxCommsBuff[tx_comms_ptr], pkt_len);
      }
      else
      {
          // Transfer complete
          comms_ep_data = 0;
          USBD_COMMS_TxCpltCallback(pTxCommsBuff, tx_comms_total_len, COMMSInEpAdd);
      }
  }else{
	pdev->ep_in[COMMSInEpAdd & 0xFU].total_length = 0U;
	/* Send ZLP */
	ret = USBD_LL_Transmit (pdev, COMMSInEpAdd, NULL, 0U);
  }

  return ret;
}

static uint8_t USBD_Comms_DataOut(USBD_HandleTypeDef *pdev, uint8_t epnum)
{
#ifdef USE_USBD_COMPOSITE
  COMMSOutEpAdd = USBD_CoreGetEPAdd(pdev, USBD_EP_OUT, USBD_EP_TYPE_BULK, COMMS_InstID);
#endif

  uint32_t rx_len = USBD_LL_GetRxDataSize(pdev, COMMSOutEpAdd);
  uint32_t packet_size = (pdev->dev_speed == USBD_SPEED_HIGH)?COMMS_HS_MAX_PACKET_SIZE:COMMS_FS_MAX_PACKET_SIZE;

  if(pUserRxBuff){
	  uint8_t* pUserRx = pUserRxBuff + rxIndex;
	  memcpy(pUserRx, pRxCommsBuff, rx_len);
	  rxIndex += rx_len;
  }

  if(read_to_idle_enabled == 1)
  {
	  // Restart timer when data is received
	  HAL_TIM_Base_Stop_IT(&htim12);
	  __HAL_TIM_SET_COUNTER(&htim12, 0); // Reset the timer counter

	  printf("start idle timer\r\n");
	  HAL_TIM_Base_Start_IT(&htim12);
  }

  // Re-arm reception
  USBD_LL_PrepareReceive(pdev, COMMS_OUT_EP, pRxCommsBuff, packet_size);
  return USBD_OK;
}

uint8_t USBD_COMMS_SendData(USBD_HandleTypeDef *pdev, uint8_t *data, uint16_t len, uint8_t ep_idx)
{
  return (uint8_t)USBD_OK;
}

uint8_t  USBD_COMMS_SetTxBuffer(USBD_HandleTypeDef *pdev, uint8_t  *pbuff, uint16_t length)
{
	uint8_t ret = USBD_OK;

	if(comms_ep_enabled == 1 && comms_ep_data==0)
	{
#ifdef USE_USBD_COMPOSITE
		/* Get the Endpoints addresses allocated for this CDC class instance */
		COMMSInEpAdd  = USBD_CoreGetEPAdd(pdev, USBD_EP_IN, USBD_EP_TYPE_BULK, COMMS_InstID);
#endif /* USE_USBD_COMPOSITE */

		USBD_LL_FlushEP(pdev, COMMSInEpAdd);
		memset((uint32_t*)pTxCommsBuff,0,USB_COMMS_MAX_SIZE/4);
		memcpy(pTxCommsBuff,pbuff,length);

        tx_comms_total_len = length;
        tx_comms_ptr = 0;

        uint16_t pkt_len = MIN((pdev->dev_speed == USBD_SPEED_HIGH)?COMMS_HS_MAX_PACKET_SIZE:COMMS_FS_MAX_PACKET_SIZE, tx_comms_total_len);

		pdev->ep_in[COMMSInEpAdd & 0xFU].total_length = tx_comms_total_len;
		comms_ep_data = 1;

		ret = USBD_LL_Transmit(pdev, COMMSInEpAdd, pTxCommsBuff, pkt_len);
	}
	else
	{
		ret = USBD_BUSY;
	}
  return ret;
}


uint8_t USBD_COMMS_Transmit(USBD_HandleTypeDef *pdev, uint8_t* Buf, uint16_t Len)
{
	USBD_COMMS_SetTxBuffer(pdev, Buf, Len);

	return (uint8_t)USBD_OK;
}

void USBD_COMMS_Idle_Timer_Handler()
{
	read_to_idle_enabled = 0;
	HAL_TIM_Base_Stop_IT(&htim12);

	if(pUserRxBuff){
		printf("comms_handle_RxCpltCallback %d \r\n", rxIndex);
		USBD_COMMS_RxCpltCallback(rxIndex);
	}else{
		printf("RX EMPTY\r\n");
		USBD_COMMS_RxCpltCallback(0);
	}

	rxIndex = 0;
	pUserRxBuff = NULL;
}

void USBD_COMMS_FlushRxBuffer()
{

}

void USBD_COMMS_ReceiveToIdle(uint8_t* Buf, uint16_t max_size)
{
	rxIndex = 0;
	rxMaxSize = max_size;
	pUserRxBuff = Buf;
	read_to_idle_enabled = 1;
}

uint8_t USBD_COMMS_RegisterRxCallback(void (*cb)(uint8_t *buf, uint16_t len)) {
    CommsRxCallback = cb;
    return USBD_OK;
}

uint8_t USBD_COMMS_RegisterInterface(USBD_HandleTypeDef *pdev, uint8_t *buffer)
{
  UNUSED(pdev);
  UNUSED(buffer);
  return (uint8_t)USBD_OK;
}

__weak void USBD_COMMS_TxCpltCallback(uint8_t *Buf, uint32_t Len, uint8_t epnum)
{
	UNUSED(Buf);
	UNUSED(Len);
	UNUSED(epnum);
}

__weak void USBD_COMMS_RxCpltCallback(uint16_t Len)
{
	UNUSED(Len);
}
