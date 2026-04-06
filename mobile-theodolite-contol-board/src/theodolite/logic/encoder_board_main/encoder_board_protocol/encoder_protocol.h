/*
 * encoder_protocol.h
 *
 *  Created on: 19 мар. 2019 г.
 *      Author: mmalenko
 */

#ifndef SRC_THEODOLITE_LOGIC_ENCODER_BOARD_ENCODER_BOARD_PROTOCOL_ENCODER_PROTOCOL_H_
#define SRC_THEODOLITE_LOGIC_ENCODER_BOARD_ENCODER_BOARD_PROTOCOL_ENCODER_PROTOCOL_H_

#include <theodolite/logic/encoder_board_main/encoder_board_protocol/encoder_protocol_interface.h>
#include <firmware/protobuf_messages_processor/protobuf_messages_processor.h>
#include <theodolite/logic/encoder_board/inclinometer/inclinometer_interface.h>
#include <theodolite/logic/encoder_board/indication/indication_interface.h>
#include <theodolite/logic/encoder_board_main/encoder/encoder_interface.h>

class EncoderBoardProtocol:
  public EncoderBoardProtocolInterface
{
public:
  EncoderBoardProtocol(InclinometerInterface &inclinometer, EncoderInterface &azimuthEncoder,
                       EncoderInterface &elevationEncoder,
                       ConnectionInterface &interface);
  virtual ~EncoderBoardProtocol()
  {

  }
  void setSendRequest();
  bool sendStateOnRequest();
  bool sendStateWithoutRequest();
  private:
  uint32_t m_bufferPointer;
  bool m_sendRequest;
  ConnectionInterface &m_interface;
  InclinometerInterface &m_inclinometer;
  EncoderInterface &m_azimuthEncoder;
  EncoderInterface &m_elevationEncoder;
  void putValueToBuffer(uint32_t value, uint_least8_t *buffer, uint32_t valueSize);
  uint32_t getCode(int32_t mills);
};

#endif /* SRC_THEODOLITE_LOGIC_ENCODER_BOARD_ENCODER_BOARD_PROTOCOL_ENCODER_PROTOCOL_H_ */
