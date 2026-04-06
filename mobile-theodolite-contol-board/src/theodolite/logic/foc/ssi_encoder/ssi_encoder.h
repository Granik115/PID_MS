/*
 * ssi_encoder.h
 *
 *  Created on: Dec 26, 2024
 *      Author: mvm
 */

#ifndef THEODOLITE_LOGIC_FOC_SSI_ENCODER_SSI_ENCODER_H_
#define THEODOLITE_LOGIC_FOC_SSI_ENCODER_SSI_ENCODER_H_

#include "../common/foc_utils.h"
#include "../common/time_utils.h"
#include "../common/base_classes/Sensor.h"

const uint32_t SSI_RESET_T_MKS = 36; //SSI data reset time
const uint32_t SSI_T_MKS = 2; //SSI clk period
const uint32_t SSI_RESET_COUNT = SSI_RESET_T_MKS * 2 / SSI_T_MKS; // timing

#define READ_BB(addr) (*(__IO uint32_t*)(addr))
#define WRITE_BB(addr, val) (*(__IO uint32_t*)(addr) = val)

typedef enum
{
  SSIBusState_RESET,
  SSIBusState_LATCH,
  SSIBusState_RECEIVING,
  SSIBusState_RELEASING
} SSIBusState;

class SSIEncoder:
  public Sensor
{
public:
  /**
   SSIEncoder class constructor
   */
  SSIEncoder(uint32_t resolution, bool hasAlarmBit, uint32_t dataPinBBAddr, uint32_t clkDataBBAddr, bool grayCode = false);

  /** encoder initialise pins */
  //void init() override;
  /**
   *  function enabling hardware interrupts for the encoder channels with provided callback functions
   *  if callback is not provided then the interrupt is not enabled
   *
   * @param doA pointer to the A channel interrupt handler function
   * @param doB pointer to the B channel interrupt handler function
   * @param doIndex pointer to the Index channel interrupt handler function
   *
   */
  //void enableInterrupts(void (*doA)() = nullptr, void (*doB)() = nullptr, void (*doIndex)() = nullptr);
  //  Encoder interrupt callback functions
  bool isrHandler();
  // Abstract functions of the Sensor class implementation
  /** get current angle (rad) */
  float getSensorAngle() override;
  /**  get current angular velocity (rad/s) */
  //float getVelocity() override;
  virtual void update() override;
  /**
   * returns 0 if it does need search for absolute zero
   * 0 - encoder without index
   * 1 - ecoder with index
   */
  int needsSearch() override;

  int32_t getEncoderCode();
  uint32_t m_encoderAngleRaw;
  private:
  uint32_t grayDecode(uint32_t data);
  uint32_t m_pinDataBBAddr;
  uint32_t m_pinClkBBAddr;
  uint32_t m_resolution;
  bool m_hasAlarm;
  SSIBusState m_ssiBusState;
  uint8_t m_clkPinVal;
  uint8_t m_currentBit;
  uint32_t m_encoderTmpData;
  uint32_t m_encoderData;
  bool m_isGray;
  bool m_isAlarm;
  bool m_update;
  volatile long pulse_counter; //!< current pulse counter
  volatile long pulse_timestamp; //!< last impulse timestamp in us
  volatile int A_active; //!< current active states of A channel
  volatile int B_active; //!< current active states of B channel
  volatile int I_active; //!< current active states of Index channel
  // velocity calculation variables
  float prev_Th, pulse_per_second;
  volatile long prev_pulse_counter, prev_timestamp_us;
};

#endif /* THEODOLITE_LOGIC_FOC_SSI_ENCODER_SSI_ENCODER_H_ */
