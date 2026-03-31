/*
 * sync_channel.cpp
 *
 *  Created on: Mar 22, 2024
 *      Author: mvm
 *  (c) 2024 NIC SPbETU
 */

#include <time_module/hw_timers/timers_api.h>
#include "sync_channel.h"

extern uint32_t getTriggerCCR(TimerTriggerSource currentSource);

SyncChannel::SyncChannel(uint32_t pulseWidth)
{
  m_riseEvent = SyncMessages_NO_EVENT;
  m_fallEvent = SyncMessages_NO_EVENT;
  m_trigger = TimerTriggerSources_INTERNAL;
  m_lastTriggerCCR = 0;
  m_originCcr = 0;
  m_pulseWidth = pulseWidth;
  m_riseCompare = 0;
  m_fallCompare = pulseWidth;
  m_riseCompareMode = ComparePulseMode_ACTIVE;
  m_relativeRiseCompare = 0;
}

void SyncChannel::setTrigger(TimerTriggerSource trigger)
{
  m_trigger = trigger;
}

void SyncChannel::setAutoreload(uint32_t arr)
{
  m_autoreload = arr;
}

void SyncChannel::notifyLogic(SyncMessages event)
{
  putSymbolToRingBuffer(event, getSyncMessagesRingBuffer());
}

PulseInfo SyncChannel::getNextPulse(uint32_t compare)
{
  PulseInfo info =
  {m_riseCompare, m_riseCompareMode};
  SyncMessages event = m_fallEvent;
  if (compare == m_riseCompare)
  {
    event = m_riseEvent;
    info.compareValue = m_fallCompare;
    info.pulseMode = ComparePulseMode_INACTIVE;
  }
  notifyLogic(event);
  return info;
}

uint32_t SyncChannel::rotateToHwPeriod(uint32_t start, uint32_t ccr)
{
  return (start + ccr) % SYNC_TIMERS_PERIOD;
}

uint32_t SyncChannel::rotateToRelativePeriod(uint32_t start, uint32_t ccr)
{
  return (start + ccr) % m_autoreload;
}

uint32_t SyncChannel::updateOriginCCR()
{
  const uint32_t currentTriggerCcr = getTriggerCCR(m_trigger);
  const bool isTriggerChanged = m_lastTriggerCCR != currentTriggerCcr;
  if (isTriggerChanged)
    m_originCcr = currentTriggerCcr;
  else
    m_originCcr = rotateToHwPeriod(m_originCcr, m_autoreload);
  m_lastTriggerCCR = currentTriggerCcr;
  return m_originCcr;
}

bool SyncChannel::isNewPeriod(uint32_t relativeCCR)
{
  if (relativeCCR <= m_relativeFallCompare)
    return true;
  else
    return (relativeCCR - m_relativeFallCompare) < MAX_CCR_DIFF;
}

int64_t SyncChannel::diffHwCcr(uint32_t first, uint32_t second)
{
  return 0;
}

/*Установка сдвига фронта СИ в единицах счетчика при старте счетчика с 0*/
void SyncChannel::setRelativeRiseCompare(uint32_t relativeCCR)
{
  if (newPeriodFlag)
    m_originCcr = updateOriginCCR();
  const uint32_t relativeFallCompare = rotateToRelativePeriod(relativeCCR, m_pulseWidth);
  newPeriodFlag = isNewPeriod(relativeFallCompare);
  uint32_t riseCompare = m_originCcr + relativeCCR;
  uint32_t addition = 0;
  if (relativeCCR <= m_relativeFallCompare)
    addition = m_autoreload;
  m_riseCompare = rotateToHwPeriod(m_originCcr, relativeCCR + addition);
  m_fallCompare = rotateToHwPeriod(m_riseCompare, m_pulseWidth);
  m_relativeRiseCompare = relativeCCR;
  m_relativeFallCompare = rotateToRelativePeriod(relativeCCR, m_pulseWidth);
  setChannelCompare(m_riseCompare);
  setChannelMode(m_riseCompareMode);
}

void SyncChannel::setConfiguration(TimerChannelConfig *config)
{
  if (!config)
    return;
  m_autoreload = config->autoreload;
  m_trigger = config->source;
  m_riseCompareMode = config->outputEnable ? ComparePulseMode_ACTIVE : ComparePulseMode_INACTIVE;
  setRelativeRiseCompare(config->ccr1);
}
