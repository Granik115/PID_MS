///*(c) 2016 NIC SPbETU
// * encoder.h
// *
// *  Created on: 17 мая 2018 г.
// *      Author: mmalenko
//(c) 2018 NIC SPbETU
// */

#ifndef SRC_THEODOLITE_LOGIC_ENCODER_BOARD_ENCODER_ENCODER_H_
#define SRC_THEODOLITE_LOGIC_ENCODER_BOARD_ENCODER_ENCODER_H_

#include "encoder_interface.h"
#include "theodolite/logic/encoder_board_main/encoder/encoder_interface.h"
#include "theodolite/logic/soft_timer/sw_timer.h"
#include "theodolite/logic/periph_bridge/gpio/gpio.h"

const uint32_t RON275_ENCODER_TIMER_PERIOD = 60000;
const uint32_t RON275_ENCODER_TIMER_OVERFLOW_MAX = 12;
const uint32_t ZERO_LED_DELAY_MS = 500;
const uint32_t OPTICAL_ENCODER_MAX_COUNT = 720000;
const uint32_t LIR_ABS_ENCODER_TIMER_PERIOD = 256;
const uint32_t LIR_ABS_ENCODER_TIMER_OVERFLOW_MAX = 8;
const uint32_t FPGA_ENCODER_TIMER_PERIOD = 0xFFFFF;
const uint32_t FPGA_ENCODER_TIMER_OVERFLOW_MAX = 1;

#if  defined(FPGA_ENCODER)
const uint32_t ENCODER_TIMER_PERIOD = FPGA_ENCODER_TIMER_PERIOD;
const uint16_t ENCODER_OVERFLOW_MAXIMUM = FPGA_ENCODER_TIMER_OVERFLOW_MAX;
#elif defined(RON_ENCODER)
const uint16_t ENCODER_TIMER_PERIOD = RON275_ENCODER_TIMER_PERIOD;
const uint16_t ENCODER_OVERFLOW_MAXIMUM = RON275_ENCODER_TIMER_OVERFLOW_MAX;
#else
const uint16_t ENCODER_TIMER_PERIOD = LIR_ABS_ENCODER_TIMER_PERIOD;
const uint16_t ENCODER_OVERFLOW_MAXIMUM = LIR_ABS_ENCODER_TIMER_OVERFLOW_MAX;
#endif

const int32_t ENCODER_CALCULATE_PERIOD_MS = 1;
const uint32_t ENCODER_HALF_TIMER_PERIOD = ENCODER_TIMER_PERIOD / 2;

class Encoder:
  public EncoderInterface
{
  int32_t m_velocityMills;
  uint32_t m_timerCounter;
  uint32_t m_encoderCount;
  uint32_t m_overflowCounter;
  theodolite_messages_AxisPosition m_position;
  Timer m_zeroDelayTimer;
  public:
  bool isZero();
  uint32_t getCount();
  void calculatePosition(const uint32_t currentTimerCount);
  void init();
  int32_t getVelocity();
  Encoder();
  virtual ~Encoder()
  {

  }
  theodolite_messages_AxisPosition getAxisPosition();
  private:
  void calculatePositionFpgaEncoder(const uint32_t currentTimerCount);
  void calculatePositionTmsQep(const uint32_t currentTimerCount);
};

EncoderInterface &getAzimithEncoderInterface();
EncoderInterface &getElevationEncoderInterface();

#endif /* SRC_THEODOLITE_LOGIC_ENCODER_BOARD_ENCODER_ENCODER_H_ */
