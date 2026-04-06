/*
 * ramp_control_test.cpp
 *
 *  Created on: 4 авг. 2017 г.
 *      Author: mmalenko
 *  (c) 2017 NIC SPbETU
 */

#include "stdlib.h"
#include <gtest/gtest.h>
#include <theodolite/logic/ramp_control/ramp_control.h>

TEST(RampControlTest, sendMessage)
{
  float delta = 0.01;
  float target = 30.001;
  RampControl rampControl = RampControl(delta,RAMPCONTROL_LINEAR);
  rampControl.setTargetValue(target);

  for (uint32_t i = 0; i < (target / delta) + 1; i++)
  {
    rampControl.calculateNextSetPoint();
  }
  ASSERT_EQ(rampControl.getSetPointValue(), target);
  ASSERT_EQ(rampControl.isEqual(), true);
  const float deltaNext = delta * 2;
  rampControl.setDelta(deltaNext);
  const float targetNext = -30;
  rampControl.setTargetValue(targetNext);
  for (uint32_t i = 0; i < ((labs(targetNext - target)) / deltaNext)+1; i++)
  {
    rampControl.calculateNextSetPoint();
  }
  ASSERT_EQ(rampControl.getSetPointValue(), targetNext);
  ASSERT_EQ(rampControl.isEqual(), true);
}

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

