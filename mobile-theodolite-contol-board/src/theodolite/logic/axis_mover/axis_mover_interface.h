///*
// * move_processor_interface.h
// *
// *      Author: mmalenko
// *(c) 2016 NIC SPbETU
// */

#ifndef LOGIC_MOVE_PROCESSING_MOVE_PROCESSOR_INTERFACE_H_
#define LOGIC_MOVE_PROCESSING_MOVE_PROCESSOR_INTERFACE_H_

#include <theodolite/logic/encoder_board/encoder_board_interface.h>
#include <theodolite/logic/pid_regulator/pid.h>
#include <theodolite/logic/pwm/pwm_interface.h>
#include "stdint.h"

namespace axis_mover_interface
{

  typedef enum
  {
    ANGLE_MODE = 1,
    VELOCITY_MODE = 2
  } MovingMode;

  typedef enum
  {
    WAIT_ENCODER_DATA,
    INITIALIZATION,
    IDLE,
    UNBLOCKING,
    ROTATION_TO_ANGLE,
    BLOCKING,
    STOPPING_MOTOR,
    VERIFY_POSITION,
    ROTATION_TO_VELOCITY,
    LOCKED_TRY_INCREMENT_ANGLE_DIRECTION,
    TEST,
    STOPPING_AT_CURRENT_ANGLE,
    MANUAL_ROTATION
  } StateType;

  typedef enum
  {
    AXIS_INCREMENT_DIRECTION,
    AXIS_DECREMENT_DIRECTION,
    AXIS_STOPED
  } AxisMoveDirection;

  const uint32_t DEGREE_TO_CODE = 2000;
  const uint32_t MIN_ANGLE = 0;

  class AxisMoverInterface
  {
  public:
    virtual void setInertionCoefficient(float)=0;
    virtual void setExplicitAngleCode(uint32_t angle, bool blocking)=0;
    virtual void setExplicitVelocity(int32_t velocity)=0;
    virtual void setUpdateRequest()=0;
    virtual bool isBrake()=0;
    virtual bool isBlockAfterRotationMode()=0;
    virtual EncoderBoardInterface& getEncoderBoard()=0;
    virtual bool isDriveActivity()=0;
    virtual StateType getState()=0;
    virtual void rotate(AxisMoveDirection)=0;
    virtual void release() = 0;
    virtual void updateOnRequest()=0;
    virtual void updateWithoutRequest()=0;
    virtual theodolite_messages_AxisPosition getAxisPosition()=0;
    virtual void setMotorRampMode(RampMode)=0;
    virtual PidRegulatorInterface &getRegulator()=0;
    virtual void runTest() = 0;
    virtual bool isLastTestPassed() = 0;
    virtual ~AxisMoverInterface()
    {

    }
  };
} //namespace axis_mover_interface

#endif /* LOGIC_MOVE_PROCESSING_MOVE_PROCESSOR_INTERFACE_H_ */
