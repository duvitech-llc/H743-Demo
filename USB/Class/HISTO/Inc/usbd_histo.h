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


#ifndef HISTO_IN_EP
#define HISTO_IN_EP                                   0x81U  /* EP1 for data IN */
#endif /* HISTO_IN_EP */

#define HISTO_FS_MAX_PACKET_SIZE         64U    /* Full-speed USB */
#define HISTO_HS_MAX_PACKET_SIZE         512U   /* High-speed USB */

extern USBD_ClassTypeDef USBD_HISTO;
#define USBD_HISTO_CLASS &USBD_HISTO

#ifdef __cplusplus
}
#endif

#endif /* __USB_HISTO_H */
