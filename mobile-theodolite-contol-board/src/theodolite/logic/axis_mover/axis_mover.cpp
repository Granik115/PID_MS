///*
// * move_processing.c
// *
// *      Author: mmalenko
// * (c) 2016 NIC SPbETU
// */
#include <theodolite/logic/sys_config/sys_cfg.h>
#include "theodolite/logic/axis_mover/axis_mover.h"
#include "theodolite/logic/assert/assert_parameter.h"
#include "math.h"

using namespace axis_mover_interface;

static const int32_t MAX_STATIC_ERROR_ROTATION_STATE = 10;
static const int32_t REGTIMER_MAX_STATIC_ERROR_PIPE = 2000; //1
static const int32_t MAX_STATIC_ERROR_IDLE_STATE = 300;

static const float MAX_DUTY_VALUE = 100;
static const uint32_t MAX_AZIMUTH_EXPLICIT_ANGLE_CODE = MAX_AZIMUTH_EXPLICIT_ANGLE
* axis_mover_interface::DEGREE_TO_CODE;
static const uint32_t MIN_AZIMUTH_EXPLICIT_ANGLE_CODE = MIN_AZIMUTH_EXPLICIT_ANGLE
* axis_mover_interface::DEGREE_TO_CODE;
static const uint32_t MAX_ELEVATION_EXPLICIT_ANGLE_CODE = MAX_ELEVATION_EXPLICIT_ANGLE
* axis_mover_interface::DEGREE_TO_CODE;
static const uint32_t MIN_ELEVATION_EXPLICIT_ANGLE_CODE = MIN_ELEVATION_EXPLICIT_ANGLE
* axis_mover_interface::DEGREE_TO_CODE;

static const float DECREMENT_ANGLE_DIR_DUTY_SIGN = 1;
static const float INCREMENT_ANGLE_DIR_DUTY_SIGN = -1;
static const int32_t AXIS_NON_ZERO_SPEED_PWM_THRESHOLD = 95; //скважность, при которой считаем, что скорость не может быть =0
static const uint32_t DEFAULT_AXIS_ANGLE = 2000 * 30U; //1 degrees
static const uint32_t TEST_STATE_TIME_MS = 5000; //время теста
static const uint32_t TEST_BLINK_TIME_MS = 100; //период тестового включения тормоза
static const int32_t REGULATION_TIMEOUT = 3000;
static const int32_t IDLE_SET_MAXIMUM_INTERVAL_SPEED = 0; //Depends 1kHz!!!

bool AxisMover::isAxisLocked()
{
  bool axisLockedPredicate = false;
  if (abs(m_motor.getModelDuty()) > AXIS_NON_ZERO_SPEED_PWM_THRESHOLD)
  {
    return getMaximumIntervalSpeed() == 0;
  }
  return axisLockedPredicate;
}

void AxisMover::checkInitialization()
{
  if (!m_isInitializationDone)
  {
#ifndef NEW_ENCODER_BOARD
    if (isAxisLocked())
    #else
    if (getAxisPosition() != theodolite_messages_AxisPosition_UNKNOWN_POSITION)
#endif
    {
      m_isInitializationDone = true;
    }
  }
}

AxisMover::AxisMover(EncoderBoardInterface & encoderBoard,
                     PidRegulatorInterface & angleRegulator,
                     PidRegulatorInterface & velocityRegulator,
                     BrakesInterface & brake,
                     PwmInterface & motor,
                     OverCurrentTriggerInterface *overCurrentTrigger) :
  m_motor(motor), m_brake(brake), m_angleRegulator(angleRegulator), m_velocityRegulator(velocityRegulator),
  m_encoderBoard(
                 encoderBoard),
  m_overCurrentTrigger(overCurrentTrigger), m_explicit_BlockAfterRotation(true), m_lastTestPassed(false),
  m_isInitializationDone(false), m_updateRequestFlag(false), m_state(axis_mover_interface::WAIT_ENCODER_DATA),
  m_explicitAngleCode(DEFAULT_AXIS_ANGLE),
  m_expectedAngleCode(0), m_testTimer(TEST_STATE_TIME_MS),
  m_testBlinkTimer(TEST_BLINK_TIME_MS),
  m_regulationTimer(REGULATION_TIMEOUT), m_mode(axis_mover_interface::ANGLE_MODE), m_explicitVelocity(0),
  m_expected_BlockAfterRotation(false)
{

}

bool AxisMover::isBlockAfterRotationMode()
{
  return m_explicit_BlockAfterRotation;
}

bool AxisMover::isBrake()
{
  return m_brake.getBrakeState() == ACTIVE && m_state == IDLE;
}

bool AxisMover::isDriveActivity()
{
  return m_motor.getTimerDuty() != 0;
}

void AxisMover::setUpdateRequest(void)
{
  m_updateRequestFlag = true;
}

uint32_t AxisMover::getCorrectAngleCode(uint32_t angleCode, uint32_t maxCode,
                                        uint32_t minCode)
{
  uint32_t returnedCode = 0;
  if (angleCode < minCode)
  {
    returnedCode = minCode;
  }
  else if (angleCode > maxCode)
  {
    returnedCode = maxCode;
  }
  else
  {
    returnedCode = angleCode;
  }
  return returnedCode;
}

void AxisMover::runTest()
{
  m_motor.disableInertion();
  m_testTimer.restart();
  m_testBlinkTimer.restart();
  m_state = axis_mover_interface::TEST;
}

bool AxisMover::isLastTestPassed()
{
  return m_lastTestPassed;
}

EncoderBoardInterface & AxisMover::getEncoderBoard()
{
  return m_encoderBoard;
}

int32_t AxisMover::getStaticError()
{
  int32_t staticError = 0;
  if (getAxisPosition() == theodolite_messages_AxisPosition_NORMAL_POSITION)
  {
    staticError = getCurrentEncoderAngle() - m_expectedAngleCode;
  }
  else if (getAxisPosition() == theodolite_messages_AxisPosition_OVER_ZERO_POSITION)
  {
    staticError = getCurrentEncoderAngle() - MAX_ANGLE
    - m_expectedAngleCode;
  }
  return staticError;
}

void AxisMover::rotate(axis_mover_interface::AxisMoveDirection direction)
{
  m_brake.deactivate();
  setMotorRampMode(RAMPCONTROL_EXPONENT);
  m_state = axis_mover_interface::MANUAL_ROTATION;
  if (direction == axis_mover_interface::AXIS_INCREMENT_DIRECTION)
  {
    m_motor.setDuty(INCREMENT_ANGLE_DIR_DUTY_SIGN * MAX_DUTY_VALUE);
  }
  else if (direction == axis_mover_interface::AXIS_DECREMENT_DIRECTION)
  {
    m_motor.setDuty(DECREMENT_ANGLE_DIR_DUTY_SIGN * MAX_DUTY_VALUE);
  }
  else if (direction == axis_mover_interface::AXIS_STOPED)
  {
    m_motor.setDuty(0);
  }
}

void AxisMover::release()
{
  setMotorRampMode(RAMPCONTROL_LINEAR);
  setInertionCoefficient(AUTOMATIC_MOVE_INERTION_TIME);
  if (m_isInitializationDone)
  {
    m_state = axis_mover_interface::STOPPING_AT_CURRENT_ANGLE;
  }
  else
  {
    m_brake.deactivate();
    m_state = axis_mover_interface::INITIALIZATION;
  }
}

axis_mover_interface::StateType AxisMover::getState()
{
  return m_state;
}

AxisMover::~AxisMover()
{

}

PidRegulatorInterface & AxisMover::getRegulator()
{
  return m_angleRegulator;
}

void AxisMover::setInertionCoefficient(float coefficient)
{
  m_motor.setLinearInertionTimeCoeff(coefficient);
}

void AxisMover::setExplicitVelocity(int32_t velocity)
{
  m_explicitVelocity = velocity;
}

void AxisMover::setExplicitAngleCode(uint32_t angle, bool blockingMode)
{
  m_explicit_BlockAfterRotation = blockingMode;
  m_explicitAngleCode = angle;
}

void AxisMover::setMotorRampMode(RampMode mode)
{
  m_motor.setRampMode(mode);
}

void AxisMover::rotationVelocityStateProcess()
{
  volatile int32_t staticError = getImmediateSpeed() - m_explicitVelocity;
  const float regValue = m_velocityRegulator.getOut(staticError) * MAX_DUTY_VALUE;
  m_motor.setDuty(m_motor.getDuty() + regValue);
}

void AxisMover::rotationAngleStateProcess()
{
  const int32_t staticError = getStaticError();
  const uint32_t staticErrorModul = labs(staticError);
  const bool axisHoldPredicate = (staticErrorModul <= MAX_STATIC_ERROR_ROTATION_STATE)
  && (getMaximumIntervalSpeed() <= IDLE_SET_MAXIMUM_INTERVAL_SPEED);
  if (axisHoldPredicate)
  {
    m_angleRegulator.reset();
    m_state = axis_mover_interface::BLOCKING;
    m_regulationTimer.stop();
    return;
  }
  const bool regulationTimerStartPredicate = staticErrorModul < REGTIMER_MAX_STATIC_ERROR_PIPE;
  if (regulationTimerStartPredicate)
  {
    if (m_regulationTimer.isRunning())
    {
      if (m_regulationTimer.expired())
      {
        m_angleRegulator.reset();
        m_regulationTimer.stop();
        if (staticErrorModul >= MAX_STATIC_ERROR_IDLE_STATE)
        {
          m_state = axis_mover_interface::STOPPING_AT_CURRENT_ANGLE;
        }
        else
        {
          m_state = axis_mover_interface::BLOCKING;
        }
      }
    }
    else
    {
      m_regulationTimer.start();
    }
  }
  if (m_expectedAngleCode != m_explicitAngleCode)
  {
    m_state = axis_mover_interface::ROTATION_TO_ANGLE;
    m_expectedAngleCode = m_explicitAngleCode;
    m_regulationTimer.stop();
    return;
  }
  const float regValue = m_angleRegulator.getOut(staticError);
  m_motor.setDuty(regValue);

  // const float angleRegValue = m_angleRegulator.getOut(staticErrorNorm);
  // volatile float velocityStaticError = static_cast<float>(getImmediateSpeed()) - (-angleRegValue);
  // const float regValue = m_velocityRegulator.getOut(velocityStaticError) * MAX_DUTY_VALUE;
  // m_motor.setDuty(m_motor.getDuty() + regValue);

}

#include "firmware/queue/queue.h"
static Queue<int32_t> speed = Queue<int32_t>(1);
static float testDuty = 0;
static bool startTest = false;
static float initalDuty = 0;
static bool angleSetTest = 0;

void AxisMover::testInitialization()
{
  if (angleSetTest)
  {

  }
  else
  {
    if (startTest)
    {
      m_motor.setDuty(testDuty);
      speed.put(getImmediateSpeed());
    }
    else
    {
      m_motor.setDuty(initalDuty);
    }
  }
}

void AxisMover::testStateProcess()
{
  if (m_overCurrentTrigger != NULL)
  {
    m_overCurrentTrigger->update();
    m_motor.disableInertion();
    m_motor.setDuty(25);
    if (m_testBlinkTimer.expired())
    {
      m_testBlinkTimer.restart();
      if (m_brake.getBrakeState() == ACTIVE)
      {
        m_brake.deactivate();
      }
      else
      {
        m_brake.activate();
      }
    }
    if (m_testTimer.expired())
    {
      m_testTimer.restart();
      m_lastTestPassed = m_overCurrentTrigger->isLatch();
      m_overCurrentTrigger->clear();
      setIdleState();
    }
  }
  else
  {
    m_lastTestPassed = false;
    setIdleState();
  }
}

void AxisMover::blockingStateProcess()
{
  if (m_expected_BlockAfterRotation && m_brake.getBrakeState() != ACTIVE)
  {
    m_brake.activate();
  }
  else
  {
    m_state = axis_mover_interface::STOPPING_MOTOR;
  }
}

void AxisMover::stopStateProcess()
{
  m_motor.setDuty(0);
  if (getMaximumIntervalSpeed() == 0 && m_motor.getModelDuty() == 0)
  {
    if (m_explicit_BlockAfterRotation && m_brake.getBrakeState() != ACTIVE)
    {
      m_brake.activate();
    }
    else
    {
      const uint32_t currentCode = getCurrentEncoderAngle();
      if (getAxisPosition() == theodolite_messages_AxisPosition_OVER_ZERO_POSITION)
      {
        setExplicitAngleCode(0, m_explicit_BlockAfterRotation);
      }
      else
      {
        setExplicitAngleCode(currentCode, m_explicit_BlockAfterRotation);
        m_expectedAngleCode = m_explicitAngleCode;
      }
      setIdleState();
    }
  }
}

void AxisMover::setVelocityRotationState()
{
  m_brake.deactivate();
  m_state = axis_mover_interface::ROTATION_TO_VELOCITY;
}

void AxisMover::setIdleState()
{
  m_motor.disableInertion();
  m_motor.setDuty(0);
  m_motor.enableInertion();
  if (m_explicit_BlockAfterRotation)
  {
    m_brake.activate();
  }
  else
  {
    m_brake.deactivate();
  }
  m_state = axis_mover_interface::IDLE;
}

void AxisMover::updateWithoutRequest()
{
  if (m_encoderBoard.isDataReliable())
  {
    checkInitialization();
  }
  switch (m_state)
  {
    case axis_mover_interface::WAIT_ENCODER_DATA:
      if (m_encoderBoard.isDataReliable())
      {
        m_state = axis_mover_interface::INITIALIZATION;
      }
      break;
    case axis_mover_interface::INITIALIZATION:
      m_motor.setDuty(MAX_DUTY_VALUE * DECREMENT_ANGLE_DIR_DUTY_SIGN);
      if (m_isInitializationDone || isAxisLocked())
      {
        m_state = axis_mover_interface::LOCKED_TRY_INCREMENT_ANGLE_DIRECTION;
        m_motor.setDuty(MAX_DUTY_VALUE * INCREMENT_ANGLE_DIR_DUTY_SIGN);
      }
      break;
    case axis_mover_interface::IDLE:
      {
      if (m_expectedAngleCode != m_explicitAngleCode || m_expected_BlockAfterRotation != m_explicit_BlockAfterRotation)
      {
        m_state = axis_mover_interface::UNBLOCKING;
        m_expectedAngleCode = m_explicitAngleCode;
        m_expected_BlockAfterRotation = m_explicit_BlockAfterRotation;
        return;
      }
      const uint32_t staticErrorModul = labs(getStaticError());
      if (staticErrorModul > MAX_STATIC_ERROR_IDLE_STATE)
      {
        m_state = axis_mover_interface::UNBLOCKING;
      }
    }
      break;
    case axis_mover_interface::UNBLOCKING:
      if (m_brake.getBrakeState() != INACTIVE)
      {
        m_brake.deactivate();
      }
      else
      {
        m_state = axis_mover_interface::ROTATION_TO_ANGLE;
      }
      break;
    case axis_mover_interface::VERIFY_POSITION:
      {
      if (labs(getStaticError()) < MAX_STATIC_ERROR_IDLE_STATE)
      {
        m_state = axis_mover_interface::IDLE;
      }
      else
      {
        m_state = axis_mover_interface::UNBLOCKING;
      }
    }
      break;
    case axis_mover_interface::STOPPING_MOTOR:
      {
      if (m_motor.getModelDuty() != 0 || getMaximumIntervalSpeed() != 0)
      {
        m_motor.disableInertion();
        m_motor.setDuty(0);
        m_motor.enableInertion();
      }
      else
      {
        m_state = axis_mover_interface::VERIFY_POSITION;
      }
    }
      break;
    case axis_mover_interface::ROTATION_TO_VELOCITY:
      rotationVelocityStateProcess();
      break;
    case axis_mover_interface::ROTATION_TO_ANGLE:
      rotationAngleStateProcess();
      break;
    case axis_mover_interface::LOCKED_TRY_INCREMENT_ANGLE_DIRECTION:
      if (getAxisPosition() == theodolite_messages_AxisPosition_NORMAL_POSITION)
      {
        if (m_mode == axis_mover_interface::ANGLE_MODE)
        {
          setIdleState();
        }
        else
        {
          setVelocityRotationState();
        }
      }
      break;
    case axis_mover_interface::TEST:
      testInitialization();
      //testStateProcess();
      break;
    case axis_mover_interface::STOPPING_AT_CURRENT_ANGLE:
      stopStateProcess();
      break;
    case axis_mover_interface::MANUAL_ROTATION:
      break;
    case axis_mover_interface::BLOCKING:
      blockingStateProcess();
      break;
    default:
      break;
  }
}

void AxisMover::updateOnRequest()
{
  if (m_updateRequestFlag == true)
  {
    m_updateRequestFlag = false;
    updateWithoutRequest();
  }
}

int32_t AzimuthAxisMover::getImmediateSpeed()
{
  return m_encoderBoard.getAzimuthImmediateSpeed();
}

bool AzimuthAxisMover::isAngleOverloaded()
{
  return m_encoderBoard.isAzimuthAngleOverloaded();
}

uint32_t AzimuthAxisMover::getMaximumIntervalSpeed()
{
  return m_encoderBoard.getAzimuthMaximumIntervalSpeed();
}

int32_t
AzimuthAxisMover::getCurrentEncoderAngle()
{
  return m_encoderBoard.getAzimuth();
}

void AzimuthAxisMover::setExplicitAngleCode(uint32_t angleCode, bool blockingMode)
{
  AxisMover::setExplicitAngleCode(getCorrectAngleCode(angleCode,
                                                      MAX_AZIMUTH_EXPLICIT_ANGLE_CODE,
                                                      MIN_AZIMUTH_EXPLICIT_ANGLE_CODE),
                                  blockingMode);
}

theodolite_messages_AxisPosition
AzimuthAxisMover::getAxisPosition()
{
  return m_encoderBoard.getAzimuthPosition();
}

int32_t
ElevationAxisMover::getCurrentEncoderAngle()
{
  return m_encoderBoard.getElevation();
}

bool ElevationAxisMover::isAngleOverloaded()
{
  return m_encoderBoard.isElevationAngleOverloaded();
}

int32_t
ElevationAxisMover::getImmediateSpeed()
{
  return m_encoderBoard.getElevationImmediateSpeed();
}

uint32_t
ElevationAxisMover::getMaximumIntervalSpeed()
{
  return m_encoderBoard.getElevationMaximumIntervalSpeed();
}

theodolite_messages_AxisPosition
ElevationAxisMover::getAxisPosition()
{
  return m_encoderBoard.getElevationPosition();
}

void ElevationAxisMover::setExplicitAngleCode(uint32_t angleCode, bool blockingMode)
{
  AxisMover::setExplicitAngleCode(getCorrectAngleCode(angleCode,
                                                      MAX_ELEVATION_EXPLICIT_ANGLE_CODE,
                                                      MIN_ELEVATION_EXPLICIT_ANGLE_CODE),
                                  blockingMode);
}

