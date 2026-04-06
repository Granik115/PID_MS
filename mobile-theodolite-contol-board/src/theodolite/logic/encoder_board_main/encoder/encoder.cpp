///*
// * encoder.cpp
// *
// *  Created on: 19 сент. 2018 г.
// *      Author: mmalenko
//   (c) 2018 NIC SPbETU
// */

#include <theodolite/logic/encoder_board_main/encoder/encoder.h>
#include <stdlib.h>

void Encoder::calculatePositionTmsQep(const uint32_t currentTimerCount)
{
  const int32_t delta = currentTimerCount - m_timerCounter;
  const int32_t deltaAbs = labs(delta);
  m_velocityMills = delta / ENCODER_CALCULATE_PERIOD_MS;
  if (!delta)
  {
    return;
  }
  const bool overflow = deltaAbs > ENCODER_HALF_TIMER_PERIOD;
  if (overflow)
  {
    int32_t sign = 1;
    const bool direction = delta > 0;
    const bool DECREMENT_DIRECTION = true;
    if (direction == DECREMENT_DIRECTION)
    {
      sign = -1;
      if (m_overflowCounter != 0)     //Counting downward
      {
        --m_overflowCounter;
      }
      else
      {
        if (m_position == theodolite_messages_AxisPosition_NORMAL_POSITION)
        {
          m_position = theodolite_messages_AxisPosition_OVER_ZERO_POSITION;
        }
        m_overflowCounter = ENCODER_OVERFLOW_MAXIMUM - 1;
      }
    }
    else
    {
      ++m_overflowCounter %= ENCODER_OVERFLOW_MAXIMUM;
      if ((m_overflowCounter == 0) && (m_position == theodolite_messages_AxisPosition_OVER_ZERO_POSITION))
      {
        m_position = theodolite_messages_AxisPosition_NORMAL_POSITION;
      }
    }
    m_velocityMills = sign * (ENCODER_TIMER_PERIOD - deltaAbs) / ENCODER_CALCULATE_PERIOD_MS;
  }
  m_timerCounter = currentTimerCount;
  m_encoderCount = m_overflowCounter * ENCODER_TIMER_PERIOD + m_timerCounter;
}

void Encoder::calculatePosition(const uint32_t currentTimerCount)
{
#if  defined(FPGA_ENCODER)
  calculatePositionFpgaEncoder(currentTimerCount);
#else
  calculatePositionTmsQep(currentTimerCount);
#endif
}

void Encoder::calculatePositionFpgaEncoder(const uint32_t currentTimerCount)
{
  calculatePositionTmsQep(currentTimerCount);
  if (m_position == theodolite_messages_AxisPosition_OVER_ZERO_POSITION)
  {
    m_encoderCount = OPTICAL_ENCODER_MAX_COUNT - (ENCODER_TIMER_PERIOD - currentTimerCount);
  }
}

theodolite_messages_AxisPosition Encoder::getAxisPosition()
{
  return m_position;
}

int32_t Encoder::getVelocity()
{
  return m_velocityMills;
}

void Encoder::init()
{
  m_zeroDelayTimer.start();
  m_velocityMills = 0;
  m_position = theodolite_messages_AxisPosition_NORMAL_POSITION;
  m_encoderCount = 0;
  m_timerCounter = 0;
  m_overflowCounter = 0;
}

bool Encoder::isZero()
{
  if (m_zeroDelayTimer.expired())
  {
    m_zeroDelayTimer.stop();
  }
  return m_zeroDelayTimer.isRunning();
}

uint32_t Encoder::getCount()
{
  return m_encoderCount;
}

Encoder::Encoder() :
  m_timerCounter(0), m_encoderCount(0), m_overflowCounter(0), m_velocityMills(0),
  m_position(theodolite_messages_AxisPosition_UNKNOWN_POSITION), m_zeroDelayTimer(ZERO_LED_DELAY_MS)
{

}

