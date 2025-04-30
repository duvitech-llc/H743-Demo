/**
  ******************************************************************************
  * @file    usbd_histo.h
  * @brief   Header file for USB Histogram Data Streaming
  ******************************************************************************
  */
#ifndef __USB_HISTO_H
#define __USB_HISTO_H

#ifdef __cplusplus
extern "C" {
#endif

#include "usbd_ioreq.h"

#define HISTO_MAX_CAMERAS           8
#define HISTO_BINS_PER_CAMERA       1024
#define HISTO_DATA_BYTES_PER_BIN    4

/* Endpoint configuration */
#define HISTO_EP_IN_1               0x81  /* EP1 IN Bulk */
#define HISTO_EP_IN_2               0x82  /* EP2 IN Bulk */
#define HISTO_EP_IN_3               0x83  /* EP3 IN Bulk */
#define HISTO_EP_IN_4               0x84  /* EP4 IN Bulk */

#define HISTO_DATA_PACKET_SIZE      (HISTO_BINS_PER_CAMERA * HISTO_DATA_BYTES_PER_BIN + 12) /* 12-byte header */
#define HISTO_PACKETS_PER_FRAME     2  /* 2 cameras per endpoint */

/* Timing configuration */
#define HISTO_TARGET_FPS            40
#define HISTO_USB_XFER_TIMEOUT      1000

typedef struct {
    uint32_t frame_number;
    uint32_t camera_id;
    uint32_t timestamp;
    uint32_t bins[HISTO_BINS_PER_CAMERA];
} HistoPacketTypeDef;

typedef struct {
    uint8_t  state;
    uint8_t  buffer_idx;
    uint16_t data_ready;
    HistoPacketTypeDef packets[HISTO_PACKETS_PER_FRAME][2]; /* Double buffering */
} USBD_Histo_HandleTypeDef;

extern USBD_ClassTypeDef USBD_HISTO;
#define USBD_HISTO_CLASS &USBD_HISTO

/* Function prototypes */
uint8_t USBD_HISTO_SendData(USBD_HandleTypeDef *pdev, uint8_t *data, uint16_t len, uint8_t ep_idx);
uint8_t USBD_HISTO_RegisterInterface(USBD_HandleTypeDef *pdev, uint8_t *buffer);

#ifdef __cplusplus
}
#endif

#endif /* __USB_HISTO_H */
