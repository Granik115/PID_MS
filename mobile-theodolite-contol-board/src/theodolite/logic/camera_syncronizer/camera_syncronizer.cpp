/*(c) 2018 NIC SPbETU
 * sync.cpp
 *
 *  Created on: 5 апр. 2018 г.
 *      Author: mmalenko
 */
#include <theodolite/logic/camera_syncronizer/camera_syncronizer.h>
#include "stddef.h"
#include "theodolite/logic/soft_timer/sw_timer.h"
#include <theodolite/logic/assert/assert_parameter.h>

CameraSyncronizer::CameraSyncronizer(CameraInterface *camera, GpioPinInterface *selectorPin,
                                     SyncTimerInterface *syncTimer)
{
  this->syncTimer = syncTimer;
  this->camera = camera;
  this->selectorPin = selectorPin;
  missExternalSyncTimer = new Timer(SYNC_MISS_DETECTION_TIMEOUT);
  syncFreq = CameraFreq_FREQ_1;
  syncType = INTERNAL;
  syncTimer->setFrequency(syncFreq);
}

void CameraSyncronizer::update()
{
  if (camera != NULL)
  {
    if (syncFreq != camera->getFrequency())
    {
      syncTimer->setFrequency(camera->getFrequency());
      syncFreq = camera->getFrequency();
    }
  }
  if (syncTimer->isExternalSyncDetected())
  {
    syncTimer->clearExternalSyncDetected();
    missExternalSyncTimer->restart();
    selectorPin->setLevel(SELECT_EXETRNAL_SYNC);
    syncType = EXTERNAL;
  }
  if (missExternalSyncTimer->expired())
  {
    missExternalSyncTimer->restart();
    selectorPin->setLevel(SELECT_INTERNAL_SYNC);
    syncType = INTERNAL;
    theodolite_messages_Error error =
    {theodolite_messages_Device_THEODOLITE, 1,
      {theodolite_messages_ErrorType_SYNCHRO_ERROR_NO_SYNC_MORE_3S}};
    getErrorStorage().sendError(error);
  }
}

void CameraSyncronizer::setSyncFreq(CameraFreq freq)
{

}

