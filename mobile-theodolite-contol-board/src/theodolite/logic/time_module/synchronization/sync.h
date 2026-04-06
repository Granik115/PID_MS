/*
 * sync.h
 *
 *  Created on: 11 мая 2016 г.
 *      Author: mmalenko
 * (c) 2015 NIC SPbETU
 */

#ifndef SRC_SYNC_SYNC_H_
#define SRC_SYNC_SYNC_H_

#include "bft.pb.h"
#include "time.h"
#include <time_module/gnss/gnss.h>
#include "time_module/digital_sev/v_sev.h"
#include "timersconfig.h"
#include "hal.h"
#include "time_module/protocol/cpuprotocol.h"

#define US_IN_SECOND         (1000000)//микросекунд в секунде!

/*Время на обработку процессором события в ед. шкалы счетчика*/
#define PROCESSING_TIME_CNT  0//((uint32_t)10000)
#define CHANNEL1_NUMBER      ((uint8_t)0)
#define CHANNEL2_NUMBER      ((uint8_t)1)
#define CHANNEL3_NUMBER      ((uint8_t)2)
#define CHANNEL4_NUMBER      ((uint8_t)3)
#define CHANNEL_BROADCAST_NUMBER   ((uint8_t)4)

#define SEV_PPS_PROTECTION_CHANNEL_NUMBER      ((uint8_t)4)
#define SEV_PPM_PROTECTION_CHANNEL_NUMBER      ((uint8_t)5)
#define GNSS_PPS_PROTECTION_CHANNEL_NUMBER     ((uint8_t)6)

#define TIM_FREQ_WITH_DEVIATION_PLUS  (TIM_FREQ_CONST+((TIM_FREQ_CONST/1000000)*FREQ_DEVIATION))
#define TIM_FREQ_WITH_DEVIATION_MINUS (TIM_FREQ_CONST-((TIM_FREQ_CONST/1000000)*FREQ_DEVIATION))
#define MINIMUM_SEV_WIDTH  (TIM_FREQ_CONST-FREQ_DEVIATION)
#define SEC_TO_REGAIN_SOURCE  ((uint8_t)5)
#define PROTECTION_WINDOW_WIDTH 210

typedef enum
{
  INIT_SYNC,
  PPM_MISS_EVENT,
  PPM_RECEIVE_EVENT,
  PPM_INPUT_ENABLE_EVENT,
  GNSS_PPS_MISS_EVENT,
  GNSS_PPS_RECEIVE_EVENT,
  GNSS_PPS_INPUT_ENABLE_EVENT,
  SEV_PPS_MISS_EVENT,
  SEV_PPS_INPUT_ENABLE_EVENT,
  SEV_PPS_RECEIVE_EVENT,
  INTERNAL_PPS_RECEIVE_EVENT,
  CHANNEL1_SYNC_EVENT,
  CHANNEL2_SYNC_EVENT,
  CHANNEL3_SYNC_EVENT,
  CHANNEL4_SYNC_EVENT,
  CHANNEL1_UPDATE_SETTING_EVENT,
  CHANNEL2_UPDATE_SETTING_EVENT,
  CHANNEL3_UPDATE_SETTING_EVENT,
  CHANNEL4_UPDATE_SETTING_EVENT,
  SyncMessages_NO_EVENT
} SyncMessages;

typedef enum
{
  DIGITAL_SEV = timesync_messages_TimeSourceType_DIGITAL_SEV,
  ANALOG_SEV = timesync_messages_TimeSourceType_ANALOG_SEV,
  GNSS = timesync_messages_TimeSourceType_GNSS,
  INTERNAL = timesync_messages_TimeSourceType_INTERNAL
} SevStatus;

typedef enum
{
  CHANNEL1 = 0,
  CHANNEL2 = 1,
  CHANNEL3 = 2,
  CHANNEL4 = 3,
  Channels_ALL_CHANNELS = 4,
  PROTECT_PPS_SEV_CHANNEL = 5,
  PROTECT_PPM_SEV_CHANNEL = 6,
  PROTECT_PPS_GNSS_CHANNEL = 7,
  INTERNAL_PPS_CHANNEL = 8,
} Channels;

typedef enum
{
  FLAG_SET,
  FLAG_RESET,
  FLAG_ACTIVE
} FlagState;

typedef enum
{
  PPS_RECEIVE,
  PPS_PROCESSED
} PpsState;

typedef enum
{
  Frequency_HERTZ_16 = 0,
  Frequency_HERTZ_8 = 1,
  Frequency_HERTZ_4 = 2,
  Frequency_HERTZ_2 = 3,
  Frequency_HERTZ_1 = 4,
  Frequency_HERTZ_0_5 = 5
} Frequency;

void syncTask(void);
RingBuffer* getSyncMessagesRingBuffer(void);
SevStatus getSevStatus(void);
void syncSetFrequency(timesync_messages_Frequency frequency);
void setSyncSource(SevStatus source);
void setShiftMode(bool mode);
void setShiftValue(uint32_t channel, uint32_t shiftUs);
uint32_t getShiftValue(uint32_t channel);
timesync_messages_Frequency getSyncPulseFrequency(void);
bool isAnalogSevAlailable(void);
bool isDigitalSevAlailable(void);
bool isGnssAlailable(void);
bool isShiftMode(void);
void setExplicitTime(time_t time);
void initSyncProcess(void);

#endif /* SRC_SYNC_SYNC_H_ */
