/*
 * pwm_test.cpp
 *
 *  Created on: 7 авг. 2017 г.
 *      Author: mmalenko
 * (c) 2017 NIC SPbETU
 */

#include <gtest/gtest.h>
#include "pwm_test.h"

TEST(PwmTest, GeneratorTest)
{
  float PWM_PERIOD_S = 0.001;
  int16_t timerMaxPeriod = 10000;
  PwmGeneratorTest pwmTest = PwmGeneratorTest(timerMaxPeriod);
  pwmTest.disableInertion();
  float duty = 30;
  pwmTest.setDuty(duty);
  pwmTest.updateWithoutRequest();
  ASSERT_EQ(pwmTest.getDuty(), duty);
  duty = -100;
  pwmTest.setDuty(duty);
  pwmTest.updateWithoutRequest();
  ASSERT_EQ(pwmTest.getDuty(), duty);
  //float time = 1;
  //pwmTest.setLinearInertionTimeCoeff(time);
  //pwmTest.enableInertion();
 // pwmTest.setDuty(100);
  //uint32_t counts = (time / PWM_PERIOD_S) + 1;
  //ASSERT_EQ(counts, 1000);
  //for (uint32_t i = 0; i <= counts; i++)
  //{
  //  pwmTest.updateWithoutRequest();
  //}
  //ASSERT_EQ(pwmTest.getDuty(), -100);
  //for (uint32_t i = 0; i <= counts; i++)
  ///{
 //   pwmTest.setUpdateRequest();
 ///   pwmTest.updateOnRequest();
  //}
  //ASSERT_LT(pwmTest.getDuty(), 1);
  //ASSERT_GT(pwmTest.getDuty(), 0);
  //for (uint32_t i = 0; i <= counts; i++)
  //{
  //  pwmTest.setUpdateRequest();
  //  pwmTest.updateOnRequest();
  //}
  //ASSERT_EQ(pwmTest.getDuty(), 100);
  //ASSERT_EQ(pwmTest.getTimerDuty(), 10000);
}

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
