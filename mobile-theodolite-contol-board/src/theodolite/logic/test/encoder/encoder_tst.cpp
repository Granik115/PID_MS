///*
// * encoder_tst.cpp
// *
// *  Created on: 19 сент. 2018 г.
// *      Author: mmalenko
// (c) 2018 NIC SPbETU
// */

#include <gtest/gtest.h>
#include <theodolite/logic/encoder_board/encoder/encoder.h>

TEST(EncoderTest,CalculatePosition)
{
  Encoder encoder;
  ASSERT_EQ(encoder.getCount(), 0);
  ASSERT_EQ(encoder.getAxisPosition(), theodolite_messages_AxisPosition_UNKNOWN_POSITION);
  ASSERT_EQ(encoder.getVelocity(), 0);
  //UPCOUNT
  for (uint32_t overflow = 0; overflow < ENCODER_OVERFLOW_MAXIMUM; overflow++)
  {
    for (uint32_t counter = 0; counter < ENCODER_TIMER_PERIOD; counter++)
    {
      encoder.calculatePosition(counter);
      ASSERT_EQ(encoder.getCount(), overflow * ENCODER_TIMER_PERIOD + counter);
    }
  }
  //check overflow
  encoder.calculatePosition(0);
  ASSERT_EQ(encoder.getCount(), 0);
  //DOWNCOUNT
  for (int32_t overflow = ENCODER_OVERFLOW_MAXIMUM - 1; overflow >= 0; overflow--)
  {
    for (int32_t counter = ENCODER_TIMER_PERIOD - 1; counter >= 0; counter--)
    {
      encoder.calculatePosition(counter);
      ASSERT_EQ(encoder.getCount(), overflow * ENCODER_TIMER_PERIOD + counter);
      ASSERT_EQ(encoder.getAxisPosition(), theodolite_messages_AxisPosition_UNKNOWN_POSITION);
    }
  }
  //check overflow
  ASSERT_EQ(encoder.getCount(), 0);
}

TEST(EncoderTest,ReferenceMark)
{
  {
    Encoder encoder;
    ASSERT_EQ(encoder.getCount(), 0);
    ASSERT_EQ(encoder.getAxisPosition(), theodolite_messages_AxisPosition_UNKNOWN_POSITION);
    ASSERT_EQ(encoder.getVelocity(), 0);
    //UPCOUNT
    for (uint32_t overflow = 0; overflow < ENCODER_OVERFLOW_MAXIMUM / 2; overflow++)
    {
      for (uint32_t counter = 0; counter < ENCODER_TIMER_PERIOD; counter++)
      {
        encoder.calculatePosition(counter);
        ASSERT_EQ(encoder.getCount(), overflow * ENCODER_TIMER_PERIOD + counter);
        ASSERT_EQ(encoder.getAxisPosition(), theodolite_messages_AxisPosition_UNKNOWN_POSITION);
      }
    }
    encoder.init();
    ASSERT_EQ(encoder.getCount(), 0);
    ASSERT_EQ(encoder.getVelocity(), 0);
    ASSERT_EQ(encoder.getAxisPosition(), theodolite_messages_AxisPosition_NORMAL_POSITION);
  }
  {
    Encoder encoder;
    //DOWNCOUNT
    for (int32_t overflow = ENCODER_OVERFLOW_MAXIMUM - 1; overflow > ENCODER_OVERFLOW_MAXIMUM / 2; overflow--)
    {
      for (int32_t counter = ENCODER_TIMER_PERIOD - 1; counter >= 0; counter--)
      {
        encoder.calculatePosition(counter);
        ASSERT_EQ(encoder.getCount(), overflow * ENCODER_TIMER_PERIOD + counter);
        ASSERT_EQ(encoder.getAxisPosition(), theodolite_messages_AxisPosition_UNKNOWN_POSITION);
      }
    }
    encoder.init();
    ASSERT_EQ(encoder.getCount(), 0);
    ASSERT_EQ(encoder.getVelocity(), 0);
    ASSERT_EQ(encoder.getAxisPosition(), theodolite_messages_AxisPosition_NORMAL_POSITION);
  }
}

TEST(EncoderTest,AxisPosition)
{
  Encoder encoder;
  encoder.init();
  //DOWNCOUNT
  for (int32_t overflow = ENCODER_OVERFLOW_MAXIMUM - 1; overflow >= 0; overflow--)
  {
    for (int32_t counter = ENCODER_TIMER_PERIOD - 1; counter >= 0; counter--)
    {
      encoder.calculatePosition(counter);
      ASSERT_EQ(encoder.getCount(), overflow * ENCODER_TIMER_PERIOD + counter);
      ASSERT_EQ(encoder.getAxisPosition(), theodolite_messages_AxisPosition_OVER_ZERO_POSITION);
    }
  }
  encoder.calculatePosition(ENCODER_TIMER_PERIOD - 1);
  encoder.calculatePosition(0);
  ASSERT_EQ(encoder.getAxisPosition(), theodolite_messages_AxisPosition_NORMAL_POSITION);
}

TEST(EncoderTest,VelocityTest)
{
  Encoder encoder;
  encoder.calculatePosition(ENCODER_TIMER_PERIOD - 1); //DECREMENT DIRECTION
  ASSERT_EQ(encoder.getVelocity(), -1);
  encoder.calculatePosition(100);
  ASSERT_EQ(encoder.getVelocity(), 101);
}

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
//  ::testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}
