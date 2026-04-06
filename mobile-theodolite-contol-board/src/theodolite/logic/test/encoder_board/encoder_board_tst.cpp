/*
 * encoder_board_tst.cpp
 *
 *  Created on: 11 апр. 2019 г.
 *      Author: mmalenko
 *  (c) 2019 NIC SPbETU
 */

#include <theodolite/logic/encoder_board/encoder_board.h>
#include <theodolite/logic/test/pc_protocol/connection.h>
#include <gtest/gtest.h>
#include <stdint.h>

TEST(EncoderBoardTest, DecodeMessage)
{
  uint_least8_t encoderStateMessage[] =
  {0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0xAA, 0xBB, 0xCC, 0xDD, 0x0, 0x1};
  Data<uint_least8_t> data =
  {encoderStateMessage, sizeof(encoderStateMessage)};
  uint_least8_t linkBuffer[16];
  uartLinkEncode(&data,
                 linkBuffer,
                 false, 0xC0);
  ConnectionMock connection;
  EncoderBoard board = EncoderBoard(connection);
  for (uint32_t i = 0; i < sizeof(linkBuffer); i++)
  {
    connection.getRxQueue()->put(linkBuffer[i]);
  }

  board.updateWithoutRequest();
  ASSERT_EQ(board.getAzimuth(), 0x141312);
  ASSERT_EQ(board.getElevation(), 0x171615);
  ASSERT_EQ(board.getInclinometerXADC(), 0xBBAA);
  ASSERT_EQ(board.getInclinometerYADC(), 0xDDCC);
  ASSERT_EQ(board.getAzimuthPosition(), static_cast<theodolite_messages_AxisPosition>(0x0));
  ASSERT_EQ(board.getElevationPosition(), static_cast<theodolite_messages_AxisPosition>(0x1));

}

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
