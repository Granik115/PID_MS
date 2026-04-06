///*
// * move_processing.h
// *
// *      Author: mmalenko
// * (c) 2016 NIC SPbETU
// */

#ifndef LOGIC_MOVE_PROCESSING_MOVE_PROCESSING_H_
#define LOGIC_MOVE_PROCESSING_MOVE_PROCESSING_H_

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

class AxisMover:
  public axis_mover_interface::AxisMoverInterface
{
  static const uint32_t MAX_ANGLE = 720000;
  bool m_explicit_BlockAfterRotation;
  bool m_expected_BlockAfterRotation;
  bool m_lastTestPassed;
  bool m_updateRequestFlag;
  bool m_isInitializationDone;
  axis_mover_interface::StateType m_state;
  axis_mover_interface::MovingMode m_mode;
  PwmInterface &m_motor;
  PidRegulatorInterface &m_angleRegulator;
  PidRegulatorInterface &m_velocityRegulator;
  BrakesInterface &m_brake;
  OverCurrentTriggerInterface *m_overCurrentTrigger;
  Timer m_regulationTimer;
  Timer m_testTimer;
  Timer m_testBlinkTimer;
  void checkInitialization();
  int32_t getMinimumSpeed(int16_t pwm);
  int32_t getMaximumSpeed(int16_t pwm);
  void setVelocityRotationState();
  void rotationVelocityStateProcess();
  void rotationAngleStateProcess();
  bool isAxisLocked();
  int32_t getStaticError();
  void testStateProcess();
  void testInitialization();
  void stopStateProcess();
  void blockingStateProcess();
  void setIdleState();
  volatile uint32_t m_explicitAngleCode;
  volatile int32_t m_expectedAngleCode;
  int32_t m_explicitVelocity;
  protected:
  uint32_t getCorrectAngleCode(uint32_t expectedAngleCode, uint32_t maxCode,
                               uint32_t minCode);
  virtual int32_t getCurrentEncoderAngle()=0;
  virtual int32_t getImmediateSpeed()=0;
  virtual bool isAngleOverloaded()=0;
  virtual uint32_t getMaximumIntervalSpeed()=0;
  EncoderBoardInterface &m_encoderBoard;
  public:
  AxisMover(EncoderBoardInterface &encoderBoard,
            PidRegulatorInterface &angleRegulator,
            PidRegulatorInterface &velocityRegulator,
            BrakesInterface &brake,
            PwmInterface &motor,
            OverCurrentTriggerInterface *overCurrentTrigger);
  virtual ~AxisMover();
  void setUpdateRequest();
  void setMotorRampMode(RampMode);
  void setExplicitAngleCode(uint32_t angle, bool blocking);
  void setExplicitVelocity(int32_t velocity);
  void rotate(axis_mover_interface::AxisMoveDirection);
  void release();
  virtual theodolite_messages_AxisPosition getAxisPosition()=0;
  bool isBrake();
  bool isBlockAfterRotationMode();
  void setInertionCoefficient(float);
  EncoderBoardInterface& getEncoderBoard();
  bool isDriveActivity();
  axis_mover_interface::StateType getState();
  void updateOnRequest();
  void updateWithoutRequest();
  PidRegulatorInterface & getRegulator();
  void runTest();
  bool isLastTestPassed();
};

class AzimuthAxisMover:
  public AxisMover
{
  void setExplicitAngleCode(uint32_t angleCode, bool blockingMode);
  int32_t getCurrentEncoderAngle();
  bool isAngleOverloaded();
  uint32_t getMaximumIntervalSpeed();
  int32_t getImmediateSpeed();
  public:
  theodolite_messages_AxisPosition getAxisPosition();
  AzimuthAxisMover(EncoderBoardInterface &encoderBoard,
                   PidRegulatorInterface &angleRegulator,
                   PidRegulatorInterface &velocityRegulator,
                   BrakesInterface &brake,
                   PwmInterface &motor,
                   OverCurrentTriggerInterface *overCurrentTrigger) :
    AxisMover(encoderBoard, angleRegulator, velocityRegulator, brake, motor, overCurrentTrigger)
  {

  }
  virtual ~AzimuthAxisMover()
  {

  }
};

class ElevationAxisMover:
  public AxisMover
{
  int32_t getCurrentEncoderAngle();
  void setExplicitAngleCode(uint32_t angleCode, bool blockingMode);
  bool isAngleOverloaded();
  uint32_t getMaximumIntervalSpeed();
  int32_t getImmediateSpeed();
  public:
  theodolite_messages_AxisPosition getAxisPosition();
  ElevationAxisMover(EncoderBoardInterface &encoderBoard,
                     PidRegulatorInterface &angleRegulator,
                     PidRegulatorInterface &velocityRegulator,
                     BrakesInterface &brake,
                     PwmInterface &motor,
                     OverCurrentTriggerInterface *overCurrentTrigger) :
    AxisMover(encoderBoard, angleRegulator, velocityRegulator, brake, motor, overCurrentTrigger)
  {

  }
  virtual ~ElevationAxisMover()
  {

  }
};

#endif /* LOGIC_MOVE_PROCESSING_MOVE_PROCESSING_H_ */
