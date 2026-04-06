///*
// * move_processing.c
// *
// *      Author: mmalenko
// * (c) 2016 NIC SPbETU
// */
#include <theodolite/logic/sys_config/sys_cfg.h>
#include "theodolite/logic/axis_mover/axis_mover_foc.h"
#include "theodolite/logic/assert/assert_parameter.h"
#include "math.h"

using namespace axis_mover_interface;

static const float MAX_STATIC_ERROR_ROTATION_STATE = 0.00002314*3;
static const float REGTIMER_MAX_STATIC_ERROR_PIPE = 0.0175 * 5;
static const float MAX_STATIC_ERROR_IDLE_STATE = 0.00002314 * 15;

static const float MAX_DUTY_VALUE = 100;
static const float DECREMENT_ANGLE_DIR_DUTY_SIGN = 1;
static const float INCREMENT_ANGLE_DIR_DUTY_SIGN = -1;
static const int32_t AXIS_NON_ZERO_SPEED_PWM_THRESHOLD = 95; //скважность, при которой считаем, что скорость не может быть =0
static const int32_t DEFAULT_AXIS_ANGLE = 2000 * 30U; //1 degrees
static const int32_t REGULATION_TIMEOUT = 10000;
static const float IDLE_SET_MAXIMUM_INTERVAL_SPEED = 0.0005;
static const uint32_t MILLS_IN_DEGREE = 3600000;

static const uint32_t MAX_AZIMUTH_EXPLICIT_ANGLE_CODE = MAX_AZIMUTH_EXPLICIT_ANGLE * MILLS_IN_DEGREE;
static const uint32_t MIN_AZIMUTH_EXPLICIT_ANGLE_CODE = MIN_AZIMUTH_EXPLICIT_ANGLE * MILLS_IN_DEGREE;

static const uint32_t MAX_ELEVATION_EXPLICIT_ANGLE_CODE = MAX_ELEVATION_EXPLICIT_ANGLE * MILLS_IN_DEGREE;
static const uint32_t MIN_ELEVATION_EXPLICIT_ANGLE_CODE = MIN_ELEVATION_EXPLICIT_ANGLE * MILLS_IN_DEGREE;

bool FOCAxisMover::isAxisLocked()
{
  return false;
}

void FOCAxisMover::checkInitialization()
{
  if (m_isInitializationDone)
    return;

  if (getAxisPosition() != theodolite_messages_AxisPosition_UNKNOWN_POSITION)
  {
    m_isInitializationDone = true;
  }
}

FOCAxisMover::FOCAxisMover(EncoderBoardInterface &encoderBoard,
                           PidRegulatorInterface &angleRegulator,
                           BrakesInterface &brake,
                           BLDCMotor &motor) :
  m_expected_BlockAfterRotation(false),
  m_lastTestPassed(false),
  m_updateRequestFlag(false),
  m_isInitializationDone(false),
  m_state(axis_mover_interface::WAIT_ENCODER_DATA),
  m_mode(axis_mover_interface::ANGLE_MODE),
  m_angleRegulator(angleRegulator),
  m_brake(brake),
  m_regulationTimer(REGULATION_TIMEOUT),
  m_explicit_BlockAfterRotation(false),
  m_motor(motor),
  m_explicitAngleMills(0),
  m_expectedAngleMills(0),
  m_expectedAngle(0),
  m_explicitVelocity(0),
  m_encoderBoard(encoderBoard)
{
}

bool FOCAxisMover::isBlockAfterRotationMode()
{
  return m_explicit_BlockAfterRotation;
}

bool FOCAxisMover::isBrake()
{
  return m_brake.getBrakeState() == ACTIVE && m_state == IDLE;
}

bool FOCAxisMover::isDriveActivity()
{
  return m_motor.enabled;
}

float FOCAxisMover::getMaximumIntervalSpeed()
{
  return abs(m_motor.shaft_velocity);
}

void FOCAxisMover::setUpdateRequest(void)
{
  m_updateRequestFlag = true;
}

uint32_t FOCAxisMover::getCorrectAngleCode(uint32_t angleCode, uint32_t maxCode,
                                           uint32_t minCode)
{
  uint32_t returnedCode = angleCode;
  if (angleCode < minCode)
    returnedCode = minCode;
  else if (angleCode > maxCode)
    returnedCode = maxCode;
  return returnedCode;
}

void FOCAxisMover::runTest()
{
  m_state = axis_mover_interface::TEST;
}

bool FOCAxisMover::isLastTestPassed()
{
  return m_lastTestPassed;
}

EncoderBoardInterface& FOCAxisMover::getEncoderBoard()
{
  return m_encoderBoard;
}

float FOCAxisMover::getStaticError()
{
  return getCurrentEncoderAngle() - m_expectedAngle;
}

void FOCAxisMover::rotate(axis_mover_interface::AxisMoveDirection direction)
{
  m_brake.deactivate();
  m_state = axis_mover_interface::MANUAL_ROTATION;
  return;
}

void FOCAxisMover::release()
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

axis_mover_interface::StateType FOCAxisMover::getState()
{
  return m_state;
}

FOCAxisMover::~FOCAxisMover()
{
}

PidRegulatorInterface& FOCAxisMover::getRegulator()
{
  return m_angleRegulator;
}

void FOCAxisMover::setInertionCoefficient(float coefficient)
{
}

void FOCAxisMover::setExplicitVelocity(int32_t velocity)
{
  m_explicitVelocity = velocity;
}

void FOCAxisMover::setExplicitAngleCode(uint32_t angle, bool blockingMode)
{
  m_explicit_BlockAfterRotation = blockingMode;
  m_explicitAngleMills = angle;
}

void FOCAxisMover::setExpectedAngle(uint32_t angle)
{
  m_expectedAngleMills = m_explicitAngleMills;
  m_expectedAngle = millsToRad(m_expectedAngleMills);
}

void FOCAxisMover::setMotorRampMode(RampMode mode)
{
}

float FOCAxisMover::getCurrentEncoderAngle()
{
  if (m_motor.sensor)
    return m_motor.sensor->getMechanicalAngle();
  return 0;
}

void FOCAxisMover::rotationVelocityStateProcess()
{
}

float FOCAxisMover::millsToRad(uint32_t mills)
{
  return (M_PI * mills) / (180 * MILLS_IN_DEGREE);
}

uint32_t FOCAxisMover::millsToCode(int32_t mills)
{
  return ((uint64_t)mills * MAX_ANGLE_CODE) / MAX_MILLS;
}

uint32_t FOCAxisMover::radToMills(float rad)
{
  return rad * 180 * M_1_PI * MILLS_IN_DEGREE;
}

float FOCAxisMover::codeToRad(uint32_t code)
{
  return (code * 2 * M_PI) / MAX_ANGLE_CODE;
}

void FOCAxisMover::moveTarget(int32_t target)
{
  if (!m_motor.enabled)
    m_motor.enable();
  m_motor.move(millsToRad(target));
}

void FOCAxisMover::rotationAngleStateProcess()
{
  const float staticError = getStaticError();
  const float staticErrorModul = abs(staticError);
  const bool axisHoldPredicate = (staticErrorModul <= MAX_STATIC_ERROR_ROTATION_STATE)
  && (getMaximumIntervalSpeed() <= IDLE_SET_MAXIMUM_INTERVAL_SPEED);
  if (axisHoldPredicate)
  {
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
        m_regulationTimer.stop();
        if (staticErrorModul >= MAX_STATIC_ERROR_IDLE_STATE)
          m_state = axis_mover_interface::STOPPING_AT_CURRENT_ANGLE;
        else
          m_state = axis_mover_interface::BLOCKING;
      }
    }
    else
    {
      m_regulationTimer.start();
    }
  }
  if (m_expectedAngleMills != m_explicitAngleMills)
  {
    m_state = axis_mover_interface::ROTATION_TO_ANGLE;
    setExpectedAngle(m_explicitAngleMills);
    m_regulationTimer.stop();
    moveTarget(m_expectedAngleMills);
    return;
  }
}

void FOCAxisMover::blockingStateProcess()
{
  if (m_expected_BlockAfterRotation && m_brake.getBrakeState() != ACTIVE)
    m_brake.activate();
  else
    m_state = axis_mover_interface::STOPPING_MOTOR;
}

void FOCAxisMover::stopStateProcess()
{
  if (m_motor.enabled)
    m_motor.disable();
  if (getMaximumIntervalSpeed() > IDLE_SET_MAXIMUM_INTERVAL_SPEED)
    return;

  if (m_explicit_BlockAfterRotation && m_brake.getBrakeState() != ACTIVE)
  {
    m_brake.activate();
  }
  else
  {
    setExplicitAngleCode(radToMills(getCurrentEncoderAngle()), m_explicit_BlockAfterRotation);
    setExpectedAngle(m_explicitAngleMills);
    setIdleState();
  }

}

void FOCAxisMover::setVelocityRotationState()
{
  m_brake.deactivate();
  m_state = axis_mover_interface::ROTATION_TO_VELOCITY;
}

void FOCAxisMover::setIdleState()
{
  if (m_motor.enabled)
    m_motor.disable();
  if (m_explicit_BlockAfterRotation)
    m_brake.activate();
  else
    m_brake.deactivate();
  m_state = axis_mover_interface::IDLE;
}

void FOCAxisMover::updateWithoutRequest()
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
      if (m_motor.motor_status == FOCMotorStatus::motor_ready)
        setIdleState();
      break;
    case axis_mover_interface::IDLE:
      if (m_expectedAngleMills != m_explicitAngleMills
      || m_expected_BlockAfterRotation != m_explicit_BlockAfterRotation)
      {
        m_state = axis_mover_interface::UNBLOCKING;
        setExpectedAngle(m_explicitAngleMills);
        m_expected_BlockAfterRotation = m_explicit_BlockAfterRotation;
      }
      break;
    case axis_mover_interface::UNBLOCKING:
      if (m_brake.getBrakeState() != INACTIVE)
        m_brake.deactivate();
      else
      {
        moveTarget(m_expectedAngleMills);
        m_state = axis_mover_interface::ROTATION_TO_ANGLE;
      }
      break;
    case axis_mover_interface::VERIFY_POSITION:
      if (abs(getStaticError()) < MAX_STATIC_ERROR_IDLE_STATE)
        m_state = axis_mover_interface::IDLE;
      else
        m_state = axis_mover_interface::UNBLOCKING;
      break;
    case axis_mover_interface::STOPPING_MOTOR:
      if (getMaximumIntervalSpeed() > IDLE_SET_MAXIMUM_INTERVAL_SPEED)
      {
        if (m_motor.enabled)
          m_motor.disable();
      }
      else
        m_state = axis_mover_interface::VERIFY_POSITION;
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
          setIdleState();
        else
          setVelocityRotationState();
      }
      break;
    case axis_mover_interface::TEST:
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

void FOCAxisMover::updateOnRequest()
{
  if (m_updateRequestFlag == true)
  {
    m_updateRequestFlag = false;
    updateWithoutRequest();
  }
}

void FOCAzimuthAxisMover::setExplicitAngleCode(uint32_t angleCode, bool blockingMode)
{
  FOCAxisMover::setExplicitAngleCode(getCorrectAngleCode(angleCode,
                                                         MAX_AZIMUTH_EXPLICIT_ANGLE_CODE,
                                                         MIN_AZIMUTH_EXPLICIT_ANGLE_CODE),
                                     blockingMode);
}

theodolite_messages_AxisPosition FOCAzimuthAxisMover::getAxisPosition()
{
  return m_encoderBoard.getAzimuthPosition();
}

int32_t FOCElevationAxisMover::getCurrentEncoderAngle()
{
  return m_encoderBoard.getElevation();
}

bool FOCElevationAxisMover::isAngleOverloaded()
{
  return m_encoderBoard.isElevationAngleOverloaded();
}

int32_t FOCElevationAxisMover::getImmediateSpeed()
{
  return m_encoderBoard.getElevationImmediateSpeed();
}

uint32_t FOCElevationAxisMover::getMaximumIntervalSpeed()
{
  return m_encoderBoard.getElevationMaximumIntervalSpeed();
}

theodolite_messages_AxisPosition FOCElevationAxisMover::getAxisPosition()
{
  return m_encoderBoard.getElevationPosition();
}

void FOCElevationAxisMover::setExplicitAngleCode(uint32_t angleCode, bool blockingMode)
{
  FOCAxisMover::setExplicitAngleCode(getCorrectAngleCode(angleCode,
                                                         MAX_ELEVATION_EXPLICIT_ANGLE_CODE,
                                                         MIN_ELEVATION_EXPLICIT_ANGLE_CODE),
                                     blockingMode);
}

