/*
 * camera.cpp
 *
 *      Author: mmalenko
 * (c) 2017 NIC SPbETU
 */

#include "camera.h"

typedef struct
{
  uint32_t commandExposition;
  uint32_t realExposition;
} Exposition;

const Exposition expositionsArray[] =
{
  {1983242, 2000000},
  {999824, 1000000},
  {499820, 500000},
  {249817, 250000},
  {124792, 125000},
  {62474, 62500},
  {31242, 31250},
  {15626, 15625},
  {7818, 7813},
  {3914, 3906},
  {1962, 1953},
  {986, 977},
  {498, 488},
  {254, 244},
  {156, 122},
  {108, 100}
};

bool Camera::sendRawCommand(uint_least8_t *buffer, uint16_t len)
{
  return sendCommand(buffer, CAMERA_COMMAND_SIZE);
}

Camera::Camera(ConnectionInterface &connector) :
  m_connector(connector)
{
  m_expositionType = CameraExpositionType_UNKNOWN;
  m_syncType = CameraSyncType_UNKNOWN;
  m_freq = CameraFreq_FREQ_UNKNOWN;
  m_gain = CameraGain_GAIN_UNKNOWN;
  m_exposition = 0;
}

bool Camera::applyChanges()
{
  uint_least8_t buff[CAMERA_COMMAND_SIZE];
  bool status = encode(buff, CAMERA_COMMAND_SIZE);
  if (status)
  {
    status = sendCommand(buff, CAMERA_COMMAND_SIZE);
  }
  return status;
}

void Camera::setGain(CameraGain gain)
{
  this->m_gain = gain;
}

void Camera::setSyncType(CameraSyncType syncType)
{
  this->m_syncType = syncType;
}
void Camera::setExpoType(CameraExpositionType expositionType)
{
  this->m_expositionType = expositionType;
}

void Camera::setFrequency(CameraFreq freq)
{
  this->m_freq = freq;
}

uint32_t Camera::getExposition() const
{
  return m_exposition;
}

CameraFreq Camera::getFrequency() const
{
  return m_freq;
}

CameraGain Camera::getGain() const
{
  return m_gain;
}

CameraExpositionType Camera::getExpositionType() const
{
  return m_expositionType;
}

CameraSyncType Camera::getCameraSyncType() const
{
  return m_syncType;
}

uint32_t Camera::getRealExposition(uint32_t exposition)
{
  for (uint32_t i = 0; i < sizeof(expositionsArray) / sizeof(Exposition); ++i)
  {
    if (expositionsArray[i].commandExposition == exposition)
    {
      return expositionsArray[i].realExposition;
    }
  }
  return exposition;
}

void Camera::setExposition(uint32_t exposition)
{
  this->m_exposition = getRealExposition(exposition);
}

uint_least8_t Camera::getModeCode(CameraSyncType syncType,
                                  CameraExpositionType expositionType)
{
  uint_least8_t code;
  switch (syncType)
  {
    case CameraSyncType_INTERNAL:
      if (expositionType == CameraExpositionType_MANUAL)
      {
        code = CAMERA_MODE_0;
      }
      else if (expositionType == CameraExpositionType_AUTOMATIC)
      {
        code = CAMERA_MODE_1;
      }
      break;
    case CameraSyncType_ORIGIN:
      if (expositionType == CameraExpositionType_MANUAL)
      {
        code = CAMERA_MODE_3;
      }
      else if (expositionType == CameraExpositionType_AUTOMATIC)
      {
        code = CAMERA_MODE_6;
      }
      break;
    case CameraSyncType_MIDDLE:
      if (expositionType == CameraExpositionType_MANUAL)
      {
        code = CAMERA_MODE_4;
      }
      else if (expositionType == CameraExpositionType_AUTOMATIC)
      {
        code = CAMERA_MODE_7;
      }
      break;
    case CameraSyncType_END:
      if (expositionType == CameraExpositionType_MANUAL)
      {
        code = CAMERA_MODE_5;
      }
      else if (expositionType == CameraExpositionType_AUTOMATIC)
      {
        code = CAMERA_MODE_8;
      }
      break;
    default:
      code = CAMERA_MODE_0;
      break;
  }
  return code;
}
uint_least8_t Camera::getFreqCode(CameraFreq freq)
{
  uint_least8_t code = (uint_least8_t)freq;
  return code;
}

uint_least8_t Camera::getGainCode(CameraGain gain)
{
  uint_least8_t code = (uint_least8_t)gain;
  return code;
}

bool Camera::sendCommand(uint_least8_t *buffer, uint_least8_t size)
{
  for (uint16_t i = 0; i < size; i++)
  {
    m_connector.send(buffer[i]);
  }
  return true;
}

bool Camera::encode(uint_least8_t *buffer, uint_least8_t size)
{
  if (size < CAMERA_COMMAND_SIZE)
  {
    return false;
  }
  uint_least8_t commandBuffer[CAMERA_COMMAND_SIZE];
  uint_least8_t cntrByteFreq = getFreqCode(m_freq);
  cntrByteFreq = (uint_least8_t)(cntrByteFreq << 4);
  uint_least8_t cntrByteMode = getModeCode(m_syncType, m_expositionType);
  uint_least8_t cntrByte = cntrByteMode | cntrByteFreq;
  commandBuffer[0] = CAMERA_COMMAND_MARKER;
  commandBuffer[1] = cntrByte;
  commandBuffer[2] = m_exposition & 0xFF;
  commandBuffer[3] = (m_exposition >> 8) & 0xFF;
  commandBuffer[4] = (m_exposition >> 16) & 0xFF;
  uint_least8_t gainByte = getGainCode(m_gain);
  commandBuffer[5] = gainByte;
  commandBuffer[6] = 0; //reserved
  commandBuffer[7] = 0; //CRC=0
  for (uint16_t i = 0; i < CAMERA_COMMAND_SIZE; i++)
  {
    buffer[i] = commandBuffer[i];
  }
  return true;
}

