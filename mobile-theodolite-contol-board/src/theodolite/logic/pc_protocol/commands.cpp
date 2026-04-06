/*
 * commands.cpp
 *
 *      Author: mmalenko
 *
 *  (c) 2017 NIC SPbETU
 */

#include "commands.h"

Commands::Commands(MoveControllerInterface &moveController, CameraInterface *camera,
                   ManualControllerInterface &manualController,
                   SystemTesterInterface *tester, StateSaverInterface &pidSaver) :
  m_testQueue(20),
  m_moveController(moveController), m_manualController(manualController), m_stateSaver(pidSaver)
{
  this->m_tester = tester;
  m_currentTest = AZIMUTH_MOVE_TEST;
  m_state = axis_mover_interface::WAIT_ENCODER_DATA;
  this->m_camera = camera;
  m_deltaAzimuth = 0;
  m_deltaElevation = 0;
}

void Commands::sendTestRunning()
{
  Test test =
  {m_currentTest, false, true};
  m_testQueue.put(test);
}

void Commands::sendTestFinished(bool isPassed)
{
  Test test =
  {m_currentTest, isPassed, false};
  m_testQueue.put(test);
}

bool Commands::saveState()
{
  theodolite_messages_State storedState = m_stateSaver.getState();
  PidRegulatorInterface &azimuthReg = m_moveController.getAzimuthAxisMover().getRegulator();
  PidRegulatorInterface &elevationReg = m_moveController.getElevationAxisMover().getRegulator();
  theodolite_messages_PidRegulatorCoeff azimuthRegCoeff =
  {azimuthReg.getKp(), azimuthReg.getKi(), azimuthReg.getKd(), azimuthReg.getKc()};
  theodolite_messages_PidRegulatorCoeff elevationRegCoeff =
  {elevationReg.getKp(), elevationReg.getKi(), elevationReg.getKd(), elevationReg.getKc()};
  const bool isStateStored = storedState.has_regulator_coeff && storedState.regulator_coeff.has_azimuth_regulator
  && storedState.regulator_coeff.has_elevation_regulator;
  if (isStateStored)
  {
    azimuthRegCoeff = storedState.regulator_coeff.azimuth_regulator;
    elevationRegCoeff = storedState.regulator_coeff.elevation_regulator;
  }
  setAzimuthPidCoeff(azimuthRegCoeff);
  setElevationPidCoeff(elevationRegCoeff);
  return true;
}

bool Commands::sendCameraRawCommand(uint_least8_t *buffer, uint16_t len)
{
  return m_camera->sendRawCommand(buffer, len);
}

void Commands::rotateToCoordinatesInEncoderValues(uint32_t azimuth, uint32_t elevation, bool blockingMode)
{
  m_moveController.setExplicitAngle(azimuth, elevation, blockingMode);
}

void Commands::setGeoDelta(uint32_t azimuth, uint32_t elevation)
{
  m_deltaAzimuth = azimuth;
  m_deltaElevation = elevation;
}

void Commands::rotateToCoordinatesInMills(uint32_t azimuth, uint32_t elevation, bool blockingMode)
{
  m_moveController.setExplicitAngle(azimuth, elevation, blockingMode);
}
void Commands::setCameraExposure(uint32_t exposure)
{
  m_camera->setExposition(exposure);
}

SystemState Commands::getState(void)
{
  SystemState state =
  {0};
  axis_mover_interface::AxisMoverInterface &azimuthAxisMover =
  m_moveController.getAzimuthAxisMover();
  EncoderBoardInterface &encoder = azimuthAxisMover.getEncoderBoard();
  state.deltaAzimuth = m_deltaAzimuth;
  state.deltaElevation = m_deltaElevation;
  state.azimuthMills = encoder.getAzimuth();
  state.elevationMills = encoder.getElevation();
 // state.azimuthEncoderValues = encoder.getAzimuth();
 // state.elevationEncoderValues = encoder.getElevation();
  state.inclinometerXADCValues = encoder.getInclinometerXADC();
  state.inclinometerYADCValues = encoder.getInclinometerYADC();
  state.inclinometerXMills = encoder.getInclinometerXMills();
  state.inclinometerYMills = encoder.getInclinometerYMills();
  state.azimuthBrake = azimuthAxisMover.isBrake();
  axis_mover_interface::AxisMoverInterface &elevationAxisMover =
  m_moveController.getElevationAxisMover();
  state.elevationBrake = elevationAxisMover.isBrake();
  state.azimuthDriveActivity = azimuthAxisMover.isDriveActivity();
  state.elevationDriveActivity = elevationAxisMover.isDriveActivity();
  state.exposure = m_camera->getExposition();
  state.frequency = m_camera->getFrequency();
  state.gain = getCameraGainValue(m_camera->getGain());
  state.manualControllerConnected = m_manualController.isControllerConnected();
  state.autoBlockMode = azimuthAxisMover.isBlockAfterRotationMode() || elevationAxisMover.isBlockAfterRotationMode();

  if (m_state != axis_mover_interface::TEST)
  {
    if (state.manualControllerConnected)
    {
      if (state.elevationDriveActivity || state.azimuthDriveActivity)
      {
        state.state = theodolite_messages_StateType_MANUAL_ROTATION;
      }
      else
      {
        state.state = theodolite_messages_StateType_MANUAL_IDLE;
      }
    }
    else
    {
      const axis_mover_interface::StateType azimuthState = azimuthAxisMover.getState();
      const axis_mover_interface::StateType elevationState = elevationAxisMover.getState();
      const bool axisRotatePredicate = (azimuthState != axis_mover_interface::IDLE)
      || (elevationState != axis_mover_interface::IDLE);
      if (axisRotatePredicate)
      {
        state.state = theodolite_messages_StateType_ROTATION;
      }
      else
      {
        state.state = theodolite_messages_StateType_IDLE;
      }
    }
  }
  else
  {
    state.state = theodolite_messages_StateType_TESTING;
  }
  state.cameraSyncType = m_camera->getCameraSyncType();
  state.cameraExpositionType = m_camera->getExpositionType();
  return state;
}

uint32_t Commands::getCameraGainValue(CameraGain gain)
{
  uint32_t retGain;
  switch (gain)
  {
    case CameraGain_GAIN_TEST:
      case CameraGain_GAIN_UNKNOWN:
      retGain = 0;
      break;
    case CameraGain_GAIN_0:
      retGain = 7;
      break;
    case CameraGain_GAIN_1:
      retGain = 6;
      break;
    case CameraGain_GAIN_2:
      retGain = 5;
      break;
    case CameraGain_GAIN_3:
      retGain = 4;
      break;
    case CameraGain_GAIN_4:
      retGain = 3;
      break;
    case CameraGain_GAIN_5:
      retGain = 2;
      break;
    case CameraGain_GAIN_6:
      retGain = 1;
      break;
  }
  return retGain;
}

CameraGain Commands::getCameraGainEnum(uint32_t gain)
{
  CameraGain retGain = CameraGain_GAIN_TEST;
  switch (gain)
  {
    case 0:
      retGain = CameraGain_GAIN_TEST;
      break;
    case 1:
      retGain = CameraGain_GAIN_6;
      break;
    case 2:
      retGain = CameraGain_GAIN_5;
      break;
    case 3:
      retGain = CameraGain_GAIN_4;
      break;
    case 4:
      retGain = CameraGain_GAIN_3;
      break;
    case 5:
      retGain = CameraGain_GAIN_2;
      break;
    case 6:
      retGain = CameraGain_GAIN_1;
      break;
    case 7:
      retGain = CameraGain_GAIN_0;
      break;
  }
  return retGain;
}

void Commands::applyCameraChanges()
{
  m_camera->applyChanges();
}

void Commands::setCameraGain(uint32_t gain)
{
  if (gain <= 7)
  {
    CameraGain cameraGain = getCameraGainEnum(gain);
    m_camera->setGain(cameraGain);
  }
}
void Commands::setCameraFrequency(CameraFreq frequency)
{
  m_camera->setFrequency(frequency);
}

void Commands::setCameraExpoType(CameraExpositionType expoType)
{
  m_camera->setExpoType(expoType);
}

void Commands::setCameraSyncType(CameraSyncType syncType)
{
  m_camera->setSyncType(syncType);
}

Queue<ControllersEvent>* Commands::getManualControllerEventQueue()
{
  return m_manualController.getManualControllerEventQueue();
}

Queue<Test>* Commands::getTestingQueue()
{
  return &m_testQueue;
}

void Commands::setUpRegulator(PidRegulatorInterface &regulator, float kp, float ki, float kd,
                              float kc)
{
  regulator.setKp(kp);
  regulator.setKi(ki);
  regulator.setKc(kc);
  regulator.setKd(kd);
  regulator.reset();
}

void Commands::setAzimuthPidCoeff(theodolite_messages_PidRegulatorCoeff pidMessage)
{
  setAzimuthPidCoeff(pidMessage.proportional_gain, pidMessage.integral_gain, pidMessage.differential_gain,
                     pidMessage.integral_correction_gain);
  theodolite_messages_State storedState = m_stateSaver.getState();
  if (storedState.has_regulator_coeff)
  {
    if (storedState.regulator_coeff.has_azimuth_regulator
    && comparePidStructure(pidMessage, storedState.regulator_coeff.azimuth_regulator))
    {
      return;
    }
    else
    {
      storedState.regulator_coeff.has_azimuth_regulator = true;
      storedState.regulator_coeff.azimuth_regulator = pidMessage;
      m_stateSaver.storeState(storedState);
    }
  }
  else
  {
    theodolite_messages_State state = theodolite_messages_State_init_default;
    state.has_regulator_coeff = true;
    state.regulator_coeff.has_azimuth_regulator = true;
    state.regulator_coeff.azimuth_regulator = pidMessage;
    m_stateSaver.storeState(state);
  }
}

void Commands::setElevationPidCoeff(theodolite_messages_PidRegulatorCoeff pidMessage)
{
  setElevationPidCoeff(pidMessage.proportional_gain, pidMessage.integral_gain, pidMessage.differential_gain,
                       pidMessage.integral_correction_gain);
  theodolite_messages_State storedState = m_stateSaver.getState();
  if (storedState.has_regulator_coeff)
  {
    if (storedState.regulator_coeff.has_elevation_regulator
    && comparePidStructure(pidMessage, storedState.regulator_coeff.elevation_regulator))
    {
      return;
    }
    else
    {
      storedState.regulator_coeff.has_elevation_regulator = true;
      storedState.regulator_coeff.elevation_regulator = pidMessage;
      m_stateSaver.storeState(storedState);
    }
  }
  else
  {
    theodolite_messages_State state = theodolite_messages_State_init_default;
    state.has_regulator_coeff = true;
    state.regulator_coeff.has_elevation_regulator = true;
    state.regulator_coeff.elevation_regulator = pidMessage;
    m_stateSaver.storeState(state);
  }
}

void Commands::setAzimuthPidCoeff(float kp, float ki, float kd, float kc)
{
  PidRegulatorInterface &regulator =
  m_moveController.getAzimuthAxisMover().getRegulator();
  setUpRegulator(regulator, kp, ki, kd, kc);
}

void Commands::fillPidStructure(theodolite_messages_PidRegulatorCoeff &inStruct,
                                theodolite_messages_PidRegulatorCoeff &outStruct)
{
  outStruct.proportional_gain = inStruct.proportional_gain;
}

bool Commands::comparePidStructure(theodolite_messages_PidRegulatorCoeff &lPid,
                                   theodolite_messages_PidRegulatorCoeff &rPid)
{
  return (lPid.proportional_gain == rPid.proportional_gain) && (lPid.integral_gain == rPid.integral_gain)
  && (lPid.differential_gain == rPid.differential_gain)
  && (lPid.integral_correction_gain == rPid.integral_correction_gain);
}

void Commands::setElevationPidCoeff(float kp, float ki, float kd, float kc)
{
  PidRegulatorInterface &regulator =
  m_moveController.getElevationAxisMover().getRegulator();
  setUpRegulator(regulator, kp, ki, kd, kc);
}

axis_mover_interface::AxisMoverInterface& Commands::getAzimuthMoveProcessor()
{
  return m_moveController.getAzimuthAxisMover();
}

axis_mover_interface::AxisMoverInterface& Commands::getElevationMoveProcessor()
{
  return m_moveController.getElevationAxisMover();
}

void Commands::updateTest()
{
  if (m_state == axis_mover_interface::TEST)
  {
    if (m_tester == NULL)
    {
      m_state = axis_mover_interface::IDLE;
      return;
    }
    if (m_currentTest == AZIMUTH_MOVE_TEST)
    {
      if (m_moveController.getAzimuthAxisMover().getState() != axis_mover_interface::TEST)
      {
        sendTestFinished(m_moveController.getAzimuthAxisMover().isLastTestPassed());
        m_currentTest = ELAVATION_MOVE_TEST;
        sendTestRunning();
        m_moveController.getElevationAxisMover().runTest();
      }
    }
    else if (m_currentTest == ELAVATION_MOVE_TEST)
    {
      if (m_moveController.getElevationAxisMover().getState() != axis_mover_interface::TEST)
      {

        sendTestFinished(m_moveController.getElevationAxisMover().isLastTestPassed());
        m_currentTest = CAMERA_UART_KEY_TEST;
        sendTestRunning();
      }
    }
    else
    {
      sendTestFinished(m_tester->isCameraConnectionTestOk());
      m_currentTest = ENCODER_LOOPBACK_TEST;
      sendTestRunning();
      sendTestFinished(m_tester->isEncoderLoopbackTestOk());
      m_currentTest = ONBOARD_FLASH_TEST;
      sendTestRunning();
      sendTestFinished(m_tester->isOnboardFlashTestOk());
      m_currentTest = ONBOARD_EEPROM_TEST;
      sendTestRunning();
      sendTestFinished(m_tester->isOnboardEepromTestOk());
      m_currentTest = INCOMING_SYNCHROIMPULSE_TEST;
      sendTestRunning();
      sendTestFinished(m_tester->isExternalSyncTestOk());
      //END TESTS
      m_currentTest = AZIMUTH_MOVE_TEST;
      m_state = axis_mover_interface::IDLE;
    }
  } //if (state == axis_mover_interface::TEST)
}

void Commands::setTestMode()
{
  m_currentTest = AZIMUTH_MOVE_TEST;
  m_moveController.getAzimuthAxisMover().runTest();
  m_state = axis_mover_interface::TEST;
  sendTestRunning();
}

