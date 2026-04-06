/*(c) 2018 NIC SPbETU
 * hw_timer_interface.h
 *
 *  Created on: 18 апр. 2018 г.
 *      Author: maxim
 */

#ifndef SRC_THEODOLITE_LOGIC_SYNC_TIMER_SYNC_TIMER_INTERFACE_H_
#define SRC_THEODOLITE_LOGIC_SYNC_TIMER_SYNC_TIMER_INTERFACE_H_

#include <theodolite/logic/camera_protocol/camera_interface.h>

class SyncTimerInterface
{
public:
  virtual bool isExternalSyncDetected()=0;
  virtual void setFrequency(CameraFreq)=0;
  virtual void setExternalSyncDetected()=0;
  virtual void clearExternalSyncDetected()=0;
  virtual ~SyncTimerInterface()
  {

  }
};

#endif /* SRC_THEODOLITE_LOGIC_SYNC_TIMER_SYNC_TIMER_INTERFACE_H_ */
