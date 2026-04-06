/*
 * fpga_api.h
 *
 *  Created on: 3 марта 2016 г.
 *      Author: mmalenko
 *      (c) 2015 NIC SPbETU
 */

#ifndef SOURCE_FPGA_FPGA_API_H_
#define SOURCE_FPGA_FPGA_API_H_

#include "stdint.h"
#include "hardware.h"
#include "time_module/synchronization/sync.h"
#include "time.h"

typedef enum
{
  SEV_PPS,
  SEV_PPM,
  GNSS_PPS
} SyncSources;

typedef enum
{
  TimerTriggerSources_INTERNAL = 0,
  TimerTriggerSources_PPS_SEV = 1,
  TimerTriggerSources_PPS_GNSS = 2
} TimerTriggerSource;

typedef enum
{
  CompareType_CCR = 0,  //На выходе появится импульс если CNT==CCR1;
  CompareType_CCR_OR_SYNC = 1  //На выходе появится импульс если сработает условие CNT==CCR1 или придет сигнал SYNC
} CompareType;

typedef struct
{
  uint32_t autoreload;
  uint32_t ccr1;
  CompareType compareType;
  uint32_t pulseNumber;
  TimerTriggerSource source;
  bool outputEnable;
  uint32_t frequencyValue;
  timesync_messages_Frequency frequency;
  uint32_t shiftUs;
  time_t time;
} TimerChannelConfig;

typedef struct
{
  uint32_t ppsGnssPeriod;
  uint32_t ppsSevPeriod;
} SevPeriod;

#define SET_MODE_CMD_SIZE           ((uint8_t)12)
#define SET_TRIGGER_CMD_SIZE        ((uint8_t)2)
#define SET_PROTECTION_STATE_SIZE   ((uint8_t)2)
#define MEASURED_DATA_SIZE          ((uint8_t)12)

SevPeriod getMeasuredPeriodes(void);
void initTimerChannel(TimerChannelConfig *config, Channels channel);

#endif /* SOURCE_FPGA_FPGA_API_H_ */
