/* $Id:time.h mmalenko  */
/*!
 * \file time.h
 * \brief
 *
 * PROJ: BFT
 *
 * (c) 2015 NIC SPbETU
 * ------------------------------------------------------------------------ */

#ifndef TIME_H_
#define TIME_H_

#include "stdint.h"
#include "time.h"

typedef struct _PulseTime
{
  uint8_t channel;
  struct tm time;
  uint32_t mkSec;
} PulseTime;

extern PulseTime commonTime;
extern PulseTime channelPulseTime;

void incrementStructMinutes(PulseTime *time);
void incrementMinutes(void);
void incrementHours(void);
void incrementSecond(PulseTime *timeStruct);
PulseTime* getCommonTimeStruct(void);
void setCommonTimeStruct(PulseTime time);
void vInitTime(void);
void vTaskSendPulseTime(void*);

#endif /* TIME_H_ */
