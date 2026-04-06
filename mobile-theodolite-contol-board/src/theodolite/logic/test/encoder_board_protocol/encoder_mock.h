/*
 * encoder_mock.h
 *
 *  Created on: 11 апр. 2019 г.
 *      Author: mmalenko
 *   (c) 2019 NIC SPbETU
 */

#ifndef SRC_THEODOLITE_LOGIC_TEST_ENCODER_BOARD_PROTOCOL_ENCODER_MOCK_H_
#define SRC_THEODOLITE_LOGIC_TEST_ENCODER_BOARD_PROTOCOL_ENCODER_MOCK_H_

#include <theodolite/logic/encoder_board/encoder/encoder_interface.h>
#include <gmock/gmock.h>

class EncoderMock:
  public EncoderInterface
{
public:
  MOCK_METHOD0(getCount,uint32_t());
  MOCK_METHOD1(calculatePosition,void(uint32_t));
  MOCK_METHOD0(init,void());
  MOCK_METHOD0(getVelocity,int32_t());
  MOCK_METHOD0(getAxisPosition,theodolite_messages_AxisPosition());
};

#endif /* SRC_THEODOLITE_LOGIC_TEST_ENCODER_BOARD_PROTOCOL_ENCODER_MOCK_H_ */
