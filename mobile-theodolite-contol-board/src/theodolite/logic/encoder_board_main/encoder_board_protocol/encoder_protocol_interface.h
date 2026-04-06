/*
 * encoder_protocol_interface.h
 *
 *  Created on: 19 мар. 2019 г.
 *      Author: mmalenko
 */

#ifndef SRC_THEODOLITE_LOGIC_ENCODER_BOARD_ENCODER_BOARD_PROTOCOL_ENCODER_PROTOCOL_INTERFACE_H_
#define SRC_THEODOLITE_LOGIC_ENCODER_BOARD_ENCODER_BOARD_PROTOCOL_ENCODER_PROTOCOL_INTERFACE_H_

class EncoderBoardProtocolInterface
{
public:
  virtual void setSendRequest()=0;
  virtual bool sendStateOnRequest()=0;
  virtual bool sendStateWithoutRequest()=0;
};

EncoderBoardProtocolInterface &getProtocolInterface();

#endif /* SRC_THEODOLITE_LOGIC_ENCODER_BOARD_ENCODER_BOARD_PROTOCOL_ENCODER_PROTOCOL_INTERFACE_H_ */
