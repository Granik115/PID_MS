///*
// * encoder_board_cpp.h
// *
// *  Created on: 12 ���. 2016 �.
// *      Author: mmalenko
// *(c) 2016 NIC SPbETU
// */

#ifndef LOGIC_ENCODER_BOARD_ENCODER_BOARD_CPP_H_
#define LOGIC_ENCODER_BOARD_ENCODER_BOARD_CPP_H_

#include <theodolite/logic/encoder_board/encoder_board_interface.h>
#include <firmware/interfaces/interfaces.h>
#include <firmware/queue/queue_interface.h>
#include <theodolite/logic/soft_timer/sw_timer.h>
#include <firmware/protobuf_messages_processor/protobuf_messages_processor.h>

const uint_least8_t ENCODER_BOARD_CODE_REQUEST_MARKER = 0xA6;
const uint_least8_t ENCODER_BOARD_CODE_REPLY_MARKER = 0x50;
const uint_least8_t ENCODER_BOARD_CODE_LENGTH = 12;
const uint_least8_t ENCODER_BOARD_CODE_CRC_SHIFT = 11;
const int32_t MAX_ANGLE_VALUE = 720000;
const uint32_t MAX_INCLINOMETER_VALUE = 4095;

class EncoderBoard:
  public EncoderBoardInterface
{
  static const uint32_t QUEUE_MASK = 0xFF;
  Timer m_parseTimeout;
  bool m_updateRequest;
  int32_t m_previousAzimuth;
  int32_t m_previousElevation;
  ConnectionInterface &m_interface;
  bool isAngleCorrect(int32_t angle);
  bool parseEncoderCode(QueueInterface<uint_least8_t> *buffer);
  bool parseEncoderCode(Data<uint_least8_t> &data);
  bool decodeBinary();
  void sendEncoderBoardCodeRequest(void);
  int32_t getInclinometerMillSecValue(int32_t encoderCode);
  bool isAngleOverloaded(int32_t);
  protected:
  int32_t m_azimuth;
  int32_t m_elevation;
  bool m_dataReliable;
  int32_t m_inclinometerXMills;
  int32_t m_inclinometerYMills;
  uint16_t m_inclinometerXADC;
  uint16_t m_inclinometerYADC;
  uint32_t m_divider;
  FastQueue<uint32_t, QUEUE_MASK> m_azimuthSpeedQueue;
  FastQueue<uint32_t, QUEUE_MASK> m_elevationSpeedQueue;
  int32_t m_azimuthImmediateSpeed;
  int32_t m_elevationImmediateSpeed;
  uint32_t m_azimuthMaximumIntervalSpeed;
  uint32_t m_elevationMaximumIntervalSpeed;
  theodolite_messages_AxisPosition m_azimuthAxisPosition;
  theodolite_messages_AxisPosition m_elevationAxisPosition;
  void calculateAzimuthAxisPosition();
  void calculateElevationAxisPosition();
  void calculateImmediateSpeed();
  uint32_t calculateMaximumIntervalSpeed(QueueInterface<uint32_t> &speedQueue, int32_t immediateSpeed);
  public:
  static const uint32_t INTERVAL_SPEED_CALC_DIVIDER = 4; //50*4 = 200ms
  int32_t getAzimuthImmediateSpeed();
  int32_t getElevationImmediateSpeed();
  bool isAzimuthAngleOverloaded();
  bool isElevationAngleOverloaded();
  uint32_t getAzimuthMaximumIntervalSpeed();
  uint32_t getElevationMaximumIntervalSpeed();
  ~EncoderBoard()
  {

  }
  static uint_least8_t calculateEncoderCodeCrc(QueueInterface<uint_least8_t> *buffer, uint16_t len)
  {
    uint_least8_t crc = 0;
    for (uint16_t i = 0; i < len; i++)
    {
      crc = crc ^ buffer->show(i);
    }
    return crc;
  }
  explicit EncoderBoard(ConnectionInterface &interface);
  int32_t getAzimuth();
  int32_t getElevation();
  theodolite_messages_AxisPosition getAzimuthPosition();
  theodolite_messages_AxisPosition getElevationPosition();
  int32_t getInclinometerXMills();
  int32_t getInclinometerYMills();
  uint16_t getInclinometerXADC();
  uint16_t getInclinometerYADC();
  bool isDataReliable();
  void setUpdateRequest();
  void updateOnRequest();
  virtual void updateWithoutRequest();
};

#endif /* LOGIC_ENCODER_BOARD_ENCODER_BOARD_CPP_H_ */
