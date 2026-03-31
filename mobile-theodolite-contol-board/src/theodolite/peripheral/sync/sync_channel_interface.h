/*
 * sync_channel_interface.h
 *
 *  Created on: 28 мар. 2024 г.
 *      Author: mvm
 *    (c) 2024 NIC SPbETU
 */

#ifndef STM32_SRC_SYNC_SYNC_CHANNEL_INTERFACE_H_
#define STM32_SRC_SYNC_SYNC_CHANNEL_INTERFACE_H_

#include "time_module/hw_timers/timers_api.h"

class SyncChannelInterface
{
public:
  virtual void setConfiguration(TimerChannelConfig *config)=0;
  virtual void pulseIsrHandler()=0;
  virtual void setTrigger(TimerTriggerSource)=0;
  virtual void setAutoreload(uint32_t)=0;
  virtual ~SyncChannelInterface()
  {

  }
};

#endif /* STM32_SRC_SYNC_SYNC_CHANNEL_INTERFACE_H_ */
