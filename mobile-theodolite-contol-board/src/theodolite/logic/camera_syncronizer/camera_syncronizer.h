/*(c) 2018 NIC SPbETU
 *
 */

#ifndef SRC_THEODOLITE_LOGIC_CAMERA_SYNCRONIZER_H_
#define SRC_THEODOLITE_LOGIC_CAMERA_SYNCRONIZER_H_

#include <theodolite/logic/camera_protocol/camera.h>
#include <theodolite/logic/soft_timer/sw_timer.h>
#include <theodolite/logic/sync_timer/sync_timer_interface.h>

class CameraSyncronizer
{
  typedef enum
  {
    INTERNAL,
    EXTERNAL
  } SyncType;

  Timer *missExternalSyncTimer;
  SyncType syncType;
  CameraFreq syncFreq;
  CameraInterface *camera;
  GpioPinInterface *selectorPin;
  SyncTimerInterface *syncTimer;
  static const int SYNC_MISS_DETECTION_TIMEOUT = 3000;
  static const bool SELECT_EXETRNAL_SYNC = false;
  static const bool SELECT_INTERNAL_SYNC = !SELECT_EXETRNAL_SYNC;
  void setSyncFreq(CameraFreq freq);
  public:
  void update();
  CameraSyncronizer(CameraInterface *, GpioPinInterface *, SyncTimerInterface *syncTimer);
};

#endif /* SRC_THEODOLITE_PERIPHERAL_SYNC_SYNC_H_ */
