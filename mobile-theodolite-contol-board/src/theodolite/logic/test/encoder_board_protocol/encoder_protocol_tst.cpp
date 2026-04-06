/*
 * encoder_protocol_tst.cpp
 *
 *  Created on: 11 апр. 2019 г.
 *      Author: mmalenko
 *  (c) 2019 NIC SPbETU
 */

#include <theodolite/logic/encoder_board/encoder_board.h>
#include <theodolite/logic/test/test_interface/test_interface.h>
#include <theodolite/logic/encoder_board/encoder_board_protocol/encoder_protocol.h>
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <stdint.h>
#include <theodolite/logic/test/encoder_board_protocol/encoder_mock.h>
#include <theodolite/logic/test/encoder_board_protocol/inclinometer_mock.h>

using ::testing::ReturnRef;
using ::testing::Return;

TEST(EncoderBoardProtocolTest, EncodeMessage)
{
  InclinometerMock inclinometer;
  EncoderMock azimuthEncoder;
  EncoderMock elevationEncoder;
  TestInterface interface;
  EXPECT_CALL(azimuthEncoder, getCount()).WillRepeatedly(Return(0x141312));
  EXPECT_CALL(elevationEncoder, getCount()).WillRepeatedly(Return(0x171615));
  EXPECT_CALL(azimuthEncoder, getAxisPosition()).WillRepeatedly(
  Return(theodolite_messages_AxisPosition_OVER_ZERO_POSITION));
  EXPECT_CALL(elevationEncoder, getAxisPosition()).WillRepeatedly(
  Return(theodolite_messages_AxisPosition_NORMAL_POSITION));
  EXPECT_CALL(inclinometer, getX()).WillRepeatedly(Return(0xBBAA));
  EXPECT_CALL(inclinometer, getY()).WillRepeatedly(Return(0xDDCC));

  EncoderBoardProtocol protocol = EncoderBoardProtocol(inclinometer, azimuthEncoder,
                                                       elevationEncoder,
                                                       interface);
  protocol.sendStateWithoutRequest();
  while (interface.getTxQueue()->getCount())
  {
    interface.getRxQueue()->put(interface.getTxQueue()->get());
  }

  EncoderBoard board = EncoderBoard(interface);
  board.updateWithoutRequest();
  ASSERT_EQ(board.getAzimuth(), 0x141312);
  ASSERT_EQ(board.getElevation(), 0x171615);
  ASSERT_EQ(board.getInclinometerXADC(), 0xBBAA);
  ASSERT_EQ(board.getInclinometerYADC(), 0xDDCC);
  ASSERT_EQ(board.getAzimuthPosition(), theodolite_messages_AxisPosition_OVER_ZERO_POSITION);
  ASSERT_EQ(board.getElevationPosition(), theodolite_messages_AxisPosition_NORMAL_POSITION);
}

int main(int argc, char **argv)
{
  ::testing::InitGoogleMock(&argc, argv);
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
