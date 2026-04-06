/*(c) 2016 NIC SPbETU
 * camera_interface.h
 *
 *      Author: mmalenko
 */

#ifndef LOGIC_CAMERA_PROTOCOL_CAMERA_INTERFACE_H_
#define LOGIC_CAMERA_PROTOCOL_CAMERA_INTERFACE_H_

#include "stdint.h"

typedef enum
{
  CameraGain_GAIN_0,
  CameraGain_GAIN_1,
  CameraGain_GAIN_2,
  CameraGain_GAIN_3,
  CameraGain_GAIN_4,
  CameraGain_GAIN_5,
  CameraGain_GAIN_6,
  CameraGain_GAIN_TEST,

  CameraGain_GAIN_UNKNOWN
} CameraGain;

typedef enum
{
  CameraFreq_FREQ_16 = 1,
  CameraFreq_FREQ_8,
  CameraFreq_FREQ_4,
  CameraFreq_FREQ_2,
  CameraFreq_FREQ_1,
  CameraFreq_FREQ_0_5,

  CameraFreq_FREQ_UNKNOWN
} CameraFreq;

typedef enum
{
  CameraExpositionType_MANUAL = 0,
  CameraExpositionType_AUTOMATIC = 1,

  CameraExpositionType_UNKNOWN
} CameraExpositionType;

typedef enum
{
  CameraSyncType_INTERNAL,
  CameraSyncType_ORIGIN,
  CameraSyncType_MIDDLE,
  CameraSyncType_END,

  CameraSyncType_UNKNOWN
} CameraSyncType;

class CameraInterface
{
public:
  virtual ~CameraInterface()
  {

  }
  virtual bool applyChanges() = 0;
  virtual void setGain(CameraGain gain)=0;
  virtual void setSyncType(CameraSyncType sync)=0;
  virtual void setExpoType(CameraExpositionType expoType)=0;
  virtual void setFrequency(CameraFreq freq)=0;
  virtual void setExposition(uint32_t exposition)=0;
  virtual bool sendRawCommand(uint_least8_t *buffer, uint16_t len)=0;
  virtual CameraFreq getFrequency() const = 0;
  virtual uint32_t getExposition() const = 0;
  virtual CameraGain getGain() const = 0;
  virtual CameraExpositionType getExpositionType() const = 0;
  virtual CameraSyncType getCameraSyncType() const = 0;
};

#endif /* LOGIC_CAMERA_PROTOCOL_CAMERA_INTERFACE_H_ */
