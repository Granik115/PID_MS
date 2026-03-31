/*
 * (c) 2017 NIC SPbETU
 * sync.cpp
 *
 *  Created on: 18 апр. 2018 г.
 *      Author: mmalenko
 */

#include <theodolite/peripheral/hal.h>
#include <theodolite/peripheral/common/gpio/gpio.h>
#include <theodolite/logic/sync_timer/sync_timer_interface.h>
#include <theodolite/logic/sync_timer/sync_timer.h>

SyncTimerInterface *syncTimer;
GpioPinInterface *syncDetectorPin;

GpioPinInterface *freq0_bit_pin;
GpioPinInterface *freq1_bit_pin;
GpioPinInterface *freq2_bit_pin;

void updateSyncTimer()
{
  static bool syncDetectorPinValueOld = false;
  bool syncDetectorPinValue = syncDetectorPin->getLevel();
  if (syncDetectorPinValue != syncDetectorPinValueOld)
  {
    syncTimer->setExternalSyncDetected();
  }
  syncDetectorPinValueOld = syncDetectorPinValue;
}

void syncConfig();

void syncConfig()
{
  //syncDetectorPin = new GpioPin(SYNC_PULSE_GPIO, SYNC_PULSE_GPIO_PIN);
  //freq0_bit_pin = new GpioPin(FREQ_0_GPIO, FREQ_0_GPIO_PIN);
  //freq1_bit_pin = new GpioPin(FREQ_1_GPIO, FREQ_1_GPIO_PIN);
  //freq2_bit_pin = new GpioPin(FREQ_2_GPIO, FREQ_2_GPIO_PIN);
}

void setFreqToPin(uint32_t freqCode);

void setFreqToPin(uint32_t freqCode)
{
  freq0_bit_pin->setLevel(static_cast<bool>(freqCode & 0x1));
  freq1_bit_pin->setLevel(static_cast<bool>(freqCode & 0x2));
  freq2_bit_pin->setLevel(static_cast<bool>(freqCode & 0x4));
}

void setSyncTimerToPeripheral(SyncTimerInterface *timer)
{
  syncTimer = timer;
}

bool SyncTimer::isExternalSyncDetected()
{
  return extSyncDetectedFlag;
}

void SyncTimer::setFrequency(CameraFreq freq)
{
  uint32_t freqCode = 0;
  switch (freq)
  {
    case CameraFreq_FREQ_0_5:
      freqCode = 0;
      break;
    case CameraFreq_FREQ_1:
      freqCode = 1;
      break;
    case CameraFreq_FREQ_2:
      freqCode = 2;
      break;
    case CameraFreq_FREQ_4:
      freqCode = 3;
      break;
    case CameraFreq_FREQ_8:
      freqCode = 4;
      break;
    case CameraFreq_FREQ_16:
      freqCode = 5;
      break;
    default:
      break;
  }
  setFreqToPin(freqCode);
}

SyncTimer::~SyncTimer()
{

}

void SyncTimer::clearExternalSyncDetected()
{
  extSyncDetectedFlag = false;
}

void SyncTimer::setExternalSyncDetected()
{
  extSyncDetectedFlag = true;
}

SyncTimer::SyncTimer()
{
  syncConfig();
  extSyncDetectedFlag = false;
}

