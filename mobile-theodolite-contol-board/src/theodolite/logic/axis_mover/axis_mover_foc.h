///*
// * move_processing.h
// *
// *      Author: mmalenko
// * (c) 2016 NIC SPbETU
// */

#ifndef LOGIC_AXIS_MOVER_FOC_H_
#define LOGIC_AXIS_MOVER_FOC_H_

#include <theodolite/logic/pwm/pwm_interface.h>
#include <theodolite/logic/axis_mover/axis_mover_interface.h>
#include <theodolite/logic/pid_regulator/pid_regulator_interface.h>
#include <theodolite/logic/axis_mover/brakes.h>
#include <theodolite/logic/ramp_control/ramp_control.h>
#include <theodolite/logic/soft_timer/sw_timer.h>
#include "stddef.h"
#include "stdlib.h"
#include <firmware/ring_buffer/ring_buffer.h>
#include <firmware/queue/fast_queue.h>
#include <theodolite/logic/external_controls/controls.h>
#include "BLDCMotor.h"
#include "foc/ssi_encoder/ssi_encoder.h"

class FOCAxisMover:
  public axis_mover_interface::AxisMoverInterface
{
  static const uint32_t MAX_MILLS = 1296000000;
  static const uint32_t MAX_ANGLE_CODE = 1 << 21;
  bool m_expected_BlockAfterRotation;
  bool m_lastTestPassed;
  bool m_updateRequestFlag;
  bool m_isInitializationDone;
  axis_mover_interface::StateType m_state;
  axis_mover_interface::MovingMode m_mode;
  PidRegulatorInterface &m_angleRegulator;
  BrakesInterface &m_brake;
  Timer m_regulationTimer;
  void checkInitialization();
  void setVelocityRotationState();
  void rotationVelocityStateProcess();
  void rotationAngleStateProcess();
  uint32_t radToMills(float rad);
  float millsToRad(uint32_t mills);
  float codeToRad(uint32_t code);
  void moveTarget(int32_t target);
  bool isAxisLocked();
  float getStaticError();
  void stopStateProcess();
  void blockingStateProcess();
  void setIdleState();
  void setExpectedAngle(uint32_t angle);
  protected:
  bool m_explicit_BlockAfterRotation;
  BLDCMotor &m_motor;
  volatile int32_t m_explicitAngleMills;
  volatile int32_t m_expectedAngleMills;
  float m_expectedAngle;
  int32_t m_explicitVelocity;
  uint32_t millsToCode(int32_t mills);
  uint32_t getCorrectAngleCode(uint32_t expectedAngleCode, uint32_t maxCode,
                               uint32_t minCode);
  float getCurrentEncoderAngle();
  float getMaximumIntervalSpeed();
  EncoderBoardInterface &m_encoderBoard;
  public:
  FOCAxisMover(EncoderBoardInterface &encoderBoard,
               PidRegulatorInterface &angleRegulator,
               BrakesInterface &brake,
               BLDCMotor &motor);
  virtual ~FOCAxisMover();
  void setUpdateRequest();
  static uint32_t codeToMills(uint32_t code)
  {
    return ((uint64_t)code * MAX_MILLS) / MAX_ANGLE_CODE;
  }
  void setMotorRampMode(RampMode);
  virtual void setExplicitAngleCode(uint32_t angle, bool blocking);
  void setExplicitVelocity(int32_t velocity);
  void rotate(axis_mover_interface::AxisMoveDirection);
  void release();
  virtual theodolite_messages_AxisPosition getAxisPosition()=0;
  virtual bool isBrake();
  bool isBlockAfterRotationMode();
  void setInertionCoefficient(float);
  EncoderBoardInterface& getEncoderBoard();
  bool isDriveActivity();
  virtual axis_mover_interface::StateType getState();
  void updateOnRequest();
  void updateWithoutRequest();
  PidRegulatorInterface& getRegulator();
  void runTest();
  bool isLastTestPassed();
};

class FOCAzimuthAxisMover:
  public FOCAxisMover
{
  void setExplicitAngleCode(uint32_t angleCode, bool blockingMode);
  public:
  theodolite_messages_AxisPosition getAxisPosition();
  FOCAzimuthAxisMover(EncoderBoardInterface &encoderBoard,
                      PidRegulatorInterface &angleRegulator,
                      BrakesInterface &brake,
                      BLDCMotor &motor) :
    FOCAxisMover(encoderBoard, angleRegulator, brake, motor)
  {

  }
  virtual ~FOCAzimuthAxisMover()
  {

  }
};

class FOCElevationAxisMover:
  public FOCAxisMover
{
  int32_t getCurrentEncoderAngle();
  void setExplicitAngleCode(uint32_t angleCode, bool blockingMode);
  bool isAngleOverloaded();
  uint32_t getMaximumIntervalSpeed();
  int32_t getImmediateSpeed();
  public:
  theodolite_messages_AxisPosition getAxisPosition();
  FOCElevationAxisMover(EncoderBoardInterface &encoderBoard,
                        PidRegulatorInterface &angleRegulator,
                        BrakesInterface &brake,
                        BLDCMotor &motor) :
    FOCAxisMover(encoderBoard, angleRegulator, brake, motor)
  {

  }
  virtual ~FOCElevationAxisMover()
  {

  }
};

class MockAxisMover:
  public FOCAxisMover
{
  SSIEncoder *m_encoder;
  public:
  theodolite_messages_AxisPosition getAxisPosition()
  {
    return theodolite_messages_AxisPosition_NORMAL_POSITION;
  }
  void setExplicitAngleCode(uint32_t angle, bool blockingMode)
  {
    m_explicitAngleMills = m_expectedAngleMills = angle;
    m_encoder->m_encoderAngleRaw = millsToCode(m_expectedAngleMills);
    m_explicit_BlockAfterRotation = blockingMode;
  }
  axis_mover_interface::StateType getState()
  {
    return axis_mover_interface::IDLE;
  }
  bool isBrake()
  {
    return m_explicit_BlockAfterRotation;
  }
  MockAxisMover(EncoderBoardInterface &encoderBoard,
                PidRegulatorInterface &angleRegulator,
                BrakesInterface &brake,
                BLDCMotor &motor,
                SSIEncoder *encoder) :
    FOCAxisMover(encoderBoard, angleRegulator, brake, motor)
  {
    m_encoder = encoder;
  }
  virtual ~MockAxisMover()
  {

  }
};

#endif /* LOGIC_AXIS_MOVER_FOC_H_ */
