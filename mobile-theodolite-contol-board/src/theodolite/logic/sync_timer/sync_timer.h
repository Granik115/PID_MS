/* (c) 2018 NIC SPbETU
 * sync_timer.h
 *
 *  Created on: 18 апр. 2018 г.
 *      Author: maxim
 */

#ifndef SRC_THEODOLITE_LOGIC_SYNC_TIMER_SYNC_TIMER_H_
#define SRC_THEODOLITE_LOGIC_SYNC_TIMER_SYNC_TIMER_H_

#include <theodolite/logic/sync_timer/sync_timer_interface.h>

void setSyncTimerToPeripheral(SyncTimerInterface *timer);
SyncTimerInterface * getSyncTimerFromPeripheral();

class SyncTimer:
  public SyncTimerInterface
{
  bool extSyncDetectedFlag;
  public:
  bool isExternalSyncDetected();
  void setFrequency(CameraFreq);
  void setExternalSyncDetected();
  virtual ~SyncTimer();
  void clearExternalSyncDetected();
  SyncTimer();
};

#endif /* SRC_THEODOLITE_LOGIC_SYNC_TIMER_SYNC_TIMER_H_ */
