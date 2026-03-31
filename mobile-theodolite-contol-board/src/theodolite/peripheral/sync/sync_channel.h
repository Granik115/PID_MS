/*
 * sync_channel.h
 *
 *  Created on: 28 мар. 2024 г.
 *      Author: mvm
 *  (c) 2024 NIC SPbETU
 */

#ifndef STM32_SRC_SYNC_SYNC_CHANNEL_H_
#define STM32_SRC_SYNC_SYNC_CHANNEL_H_

#include "time_module/synchronization/sync.h"
#include <stdint.h>
#include "hardware.h"
#include "sync_channel_interface.h"

const uint32_t SYNC_PULSE_WIDTH = static_cast<uint32_t>(TIM_FREQ_CONST * 0.0105); // 10ms
const uint32_t MAX_CCR_DIFF = static_cast<uint32_t>(SYNC_PULSE_WIDTH);
const uint32_t PROCESSING_TIME = MAX_CCR_DIFF;

typedef enum
{
  ComparePulseMode_ACTIVE = LL_TIM_OCMODE_ACTIVE,
  ComparePulseMode_INACTIVE = LL_TIM_OCMODE_INACTIVE,
} ComparePulseMode;

typedef struct
{
  uint32_t compareValue;
  ComparePulseMode pulseMode;
} PulseInfo;

class SyncChannel:
  public SyncChannelInterface
{
public:
  SyncChannel(uint32_t pulseWidth);
  ~SyncChannel()
  {

  }
  virtual void pulseIsrHandler(void)=0;
  void setAutoreload(uint32_t);
  TimerTriggerSource getTrigger();
  void setConfiguration(TimerChannelConfig *config);
  void setTrigger(TimerTriggerSource);
  bool isNewDebug[100];
  uint32_t debug_ccr[100];
  uint32_t debug_trigger[100];
  uint32_t debug_tim2cnt[100];
  uint32_t debug_tim5cnt[100];
  uint32_t debug_counter;
  protected:
  virtual void setChannelCompare(uint32_t)=0;
  virtual void setChannelMode(ComparePulseMode)=0;
  PulseInfo getNextPulse(uint32_t start);
  SyncMessages m_riseEvent;
  SyncMessages m_fallEvent;
  uint32_t m_riseCompare;
  private:
  uint32_t m_originCcr;
  uint32_t m_lastTriggerCCR;
  uint32_t m_pulseWidth;
  uint32_t m_relativeRiseCompare;
  uint32_t m_relativeFallCompare;
  uint32_t m_shiftUs;
  bool newPeriodFlag;
  ComparePulseMode m_riseCompareMode;
  uint32_t m_fallCompare;
  uint32_t m_autoreload = TIM_FREQ_CONST;
  TimerTriggerSource m_trigger;
  void setRelativeRiseCompare(uint32_t);
  bool isNewPeriod(uint32_t);
  uint32_t getChannelCompare();
  uint32_t rotateToHwPeriod(uint32_t start, uint32_t ccr);
  uint32_t rotateToRelativePeriod(uint32_t start, uint32_t ccr);
  uint32_t updateOriginCCR();
  int64_t diffHwCcr(uint32_t, uint32_t);
  void notifyLogic(SyncMessages);
};

class SyncChannel1:
  public SyncChannel
{
public:
  SyncChannel1(uint32_t pulseWidth) :
    SyncChannel(pulseWidth)
  {
    m_riseEvent = CHANNEL1_SYNC_EVENT;
    m_fallEvent = CHANNEL1_UPDATE_SETTING_EVENT;
  }
  void pulseIsrHandler()
  {
    if (LL_TIM_IsActiveFlag_CC1(TIM2))
    {
      const PulseInfo nextCompare = getNextPulse(LL_TIM_OC_GetCompareCH1(TIM2));
      LL_TIM_OC_SetCompareCH1(TIM2, nextCompare.compareValue);
      LL_TIM_OC_SetMode(TIM2, LL_TIM_CHANNEL_CH1, nextCompare.pulseMode);
      LL_TIM_ClearFlag_CC1(TIM2);
      LL_TIM_ClearFlag_CC1OVR(TIM2);
    }
  }
private:
  void setChannelMode(ComparePulseMode mode)
  {
    LL_TIM_OC_SetMode(TIM2, LL_TIM_CHANNEL_CH1, mode);
  }

  void setChannelCompare(uint32_t ccr)
  {
    LL_TIM_OC_SetCompareCH1(TIM2, ccr);
  }

};

class SyncChannel2:
  public SyncChannel
{
public:
  SyncChannel2(uint32_t pulseWidth) :
    SyncChannel(pulseWidth)
  {
    m_riseEvent = CHANNEL2_SYNC_EVENT;
    m_fallEvent = CHANNEL2_UPDATE_SETTING_EVENT;
  }
  void pulseIsrHandler()
  {
    if (LL_TIM_IsActiveFlag_CC2(TIM2))
    {
      const PulseInfo nextCompare = getNextPulse(LL_TIM_OC_GetCompareCH2(TIM2));
      LL_TIM_OC_SetCompareCH2(TIM2, nextCompare.compareValue);
      LL_TIM_OC_SetMode(TIM2, LL_TIM_CHANNEL_CH2, nextCompare.pulseMode);
      LL_TIM_ClearFlag_CC2(TIM2);
      LL_TIM_ClearFlag_CC2OVR(TIM2);
    }
  }
private:
  void setChannelMode(ComparePulseMode mode)
  {
    LL_TIM_OC_SetMode(TIM2, LL_TIM_CHANNEL_CH2, mode);
  }

  void setChannelCompare(uint32_t ccr)
  {
    LL_TIM_OC_SetCompareCH2(TIM2, ccr);
  }
};

class SyncChannel3:
  public SyncChannel
{
public:
  SyncChannel3(uint32_t pulseWidth) :
    SyncChannel(pulseWidth)
  {
    m_riseEvent = CHANNEL3_SYNC_EVENT;
    m_fallEvent = CHANNEL3_UPDATE_SETTING_EVENT;
  }
  void pulseIsrHandler()
  {
    if (LL_TIM_IsActiveFlag_CC3(TIM2))
    {
      const PulseInfo nextCompare = getNextPulse(LL_TIM_OC_GetCompareCH3(TIM2));
      LL_TIM_OC_SetCompareCH3(TIM2, nextCompare.compareValue);
      LL_TIM_OC_SetMode(TIM2, LL_TIM_CHANNEL_CH3, nextCompare.pulseMode);
      LL_TIM_ClearFlag_CC3(TIM2);
      LL_TIM_ClearFlag_CC3OVR(TIM2);
    }
  }
private:
  void setChannelMode(ComparePulseMode mode)
  {
    LL_TIM_OC_SetMode(TIM2, LL_TIM_CHANNEL_CH3, mode);
  }

  void setChannelCompare(uint32_t ccr)
  {
    LL_TIM_OC_SetCompareCH3(TIM2, ccr);
  }
};

class SyncChannel4:
  public SyncChannel
{
public:
  SyncChannel4(uint32_t pulseWidth) :
    SyncChannel(pulseWidth)
  {
    m_riseEvent = CHANNEL4_SYNC_EVENT;
    m_fallEvent = CHANNEL4_UPDATE_SETTING_EVENT;
  }
  void pulseIsrHandler()
  {
    if (LL_TIM_IsActiveFlag_CC4(TIM2))
    {
      const PulseInfo nextCompare = getNextPulse(LL_TIM_OC_GetCompareCH4(TIM2));
      LL_TIM_OC_SetCompareCH4(TIM2, nextCompare.compareValue);
      LL_TIM_OC_SetMode(TIM2, LL_TIM_CHANNEL_CH4, nextCompare.pulseMode);
      LL_TIM_ClearFlag_CC4(TIM2);
      LL_TIM_ClearFlag_CC4OVR(TIM2);
    }
  }
private:
  void setChannelMode(ComparePulseMode mode)
  {
    LL_TIM_OC_SetMode(TIM2, LL_TIM_CHANNEL_CH4, mode);
  }

  void setChannelCompare(uint32_t ccr)
  {
    LL_TIM_OC_SetCompareCH4(TIM2, ccr);
  }
};

class InternalSyncChannel:
  public SyncChannel
{
public:
  InternalSyncChannel(uint32_t pulseWidth) :
    SyncChannel(pulseWidth)
  {
    m_riseEvent = SyncMessages_NO_EVENT;
    m_fallEvent = INTERNAL_PPS_RECEIVE_EVENT;
  }
  void pulseIsrHandler()
  {
    if (LL_TIM_IsActiveFlag_CC3(TIM5))
    {
      const uint32_t compare = LL_TIM_OC_GetCompareCH3(TIM5);
      const PulseInfo nextCompare = getNextPulse(compare);
      LL_TIM_OC_SetCompareCH3(TIM5, nextCompare.compareValue);
      LL_TIM_OC_SetMode(TIM5, LL_TIM_CHANNEL_CH3, nextCompare.pulseMode);
      LL_TIM_ClearFlag_CC3(TIM5);
      LL_TIM_ClearFlag_CC3OVR(TIM5);
    }
  }
private:
  void setChannelMode(ComparePulseMode mode)
  {
    LL_TIM_OC_SetMode(TIM5, LL_TIM_CHANNEL_CH3, mode);
  }

  void setChannelCompare(uint32_t ccr)
  {
    LL_TIM_OC_SetCompareCH3(TIM5, ccr);
  }
};

#endif /* STM32_SRC_SYNC_SYNC_CHANNEL_H_ */
