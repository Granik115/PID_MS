/*(c) 2016 NIC SPbETU
 * camera.h
 *
 *      Author: mmalenko
 *(c) 2016 NIC SPbETU
 */

#ifndef LOGIC_CAMERA_PROTOCOL_CAMERA_H_
#define LOGIC_CAMERA_PROTOCOL_CAMERA_H_

#include "theodolite/logic/camera_protocol/camera_interface.h"
#include "theodolite/logic/software_uart/sw_uart.h"

const uint_least8_t CAMERA_MODE_0 = 0x0;
const uint_least8_t CAMERA_MODE_1 = 0x1;
const uint_least8_t CAMERA_MODE_2 = 0x2;
const uint_least8_t CAMERA_MODE_3 = 0x3;
const uint_least8_t CAMERA_MODE_4 = 0x4;
const uint_least8_t CAMERA_MODE_5 = 0x5;
const uint_least8_t CAMERA_MODE_6 = 0x6;
const uint_least8_t CAMERA_MODE_7 = 0x7;
const uint_least8_t CAMERA_MODE_8 = 0x8;

const uint_least8_t CAMERA_FREQ_16 = 0x1;
const uint_least8_t CAMERA_FREQ_8 = 0x2;
const uint_least8_t CAMERA_FREQ_4 = 0x3;
const uint_least8_t CAMERA_FREQ_2 = 0x4;
const uint_least8_t CAMERA_FREQ_1 = 0x5;
const uint_least8_t CAMERA_FREQ_0_5 = 0x6;

const uint_least8_t CAMERA_GAIN_0 = 0x0;
const uint_least8_t CAMERA_GAIN_1 = 0x1;
const uint_least8_t CAMERA_GAIN_2 = 0x2;
const uint_least8_t CAMERA_GAIN_3 = 0x3;
const uint_least8_t CAMERA_GAIN_4 = 0x4;
const uint_least8_t CAMERA_GAIN_5 = 0x5;
const uint_least8_t CAMERA_GAIN_6 = 0x6;
const uint_least8_t CAMERA_GAIN_TEST = 0x7;

const uint_least8_t CAMERA_COMMAND_MARKER = 0xA2;
const uint_least8_t CAMERA_COMMAND_SIZE = 8;

class Camera:
  public CameraInterface
{
public:
  bool sendRawCommand(uint_least8_t *buffer, uint16_t len);
  bool applyChanges();
  explicit Camera(ConnectionInterface &connector);
  void setGain(CameraGain gain);
  void setSyncType(CameraSyncType syncType);
  void setExpoType(CameraExpositionType expositionType);
  void setFrequency(CameraFreq freq);
  void setExposition(uint32_t exposition);
  uint32_t getExposition() const;
  CameraGain getGain() const;
  CameraExpositionType getExpositionType() const;
  CameraSyncType getCameraSyncType() const;
  CameraFreq getFrequency() const;
  private:
  ConnectionInterface &m_connector;
  CameraGain m_gain;
  CameraSyncType m_syncType;
  CameraExpositionType m_expositionType;
  CameraFreq m_freq;
  uint32_t m_exposition;
  bool encode(uint_least8_t *buffer, uint_least8_t size);
  bool sendCommand(uint_least8_t *buffer, uint_least8_t size);
  uint_least8_t getModeCode(CameraSyncType, CameraExpositionType);
  uint_least8_t getFreqCode(CameraFreq);
  uint_least8_t getGainCode(CameraGain);
  uint32_t getRealExposition(uint32_t exposition);
};

#endif /* LOGIC_CAMERA_PROTOCOL_CAMERA_H_ */
