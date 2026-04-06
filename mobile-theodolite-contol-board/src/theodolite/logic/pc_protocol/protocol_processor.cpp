/*
 * protocol_parser.cpp
 *
 *      Author: mmalenko
 * (c) 2016 NIC SPbETU
 */

#include <firmware/link_level/uart_link.h>
#include <firmware/third-party/nanopb/pb_encode.h>
#include <firmware/third-party/nanopb/pb_decode.h>
#include <theodolite/logic/pc_protocol/protocol_processor.h>
#include <theodolite/logic/assert/assert_parameter.h>

ErrorsStorage<theodolite_messages_Error> errorsStorage;

ErrorsStorage<theodolite_messages_Error>& getErrorStorage()
{
  return errorsStorage;
}

bool ProtocolProcessor::sendAck(uint32_t requestId)
{
  theodolite_messages_CommandEnvelope message =
  {0};
  message.has_ack = true;
  message.request_id = requestId;
  m_replyCommandLedPin->setLevel(!m_replyCommandLedPin->getLevel());
  return m_messageProcessor->sendMessage(message);
}

bool ProtocolProcessor::sendNak(uint32_t requestId)
{
  theodolite_messages_CommandEnvelope message =
  {0};
  message.request_id = requestId;
  message.has_nak = true;
  m_replyCommandLedPin->setLevel(!m_replyCommandLedPin->getLevel());
  return m_messageProcessor->sendMessage(message);
}

ProtocolProcessor::ProtocolProcessor(CommandsInterface &commands,
                                     ConnectionInterface &interface,
                                     GpioPinInterface *receiveCommandLedPin,
                                     GpioPinInterface *replyCommandLedPin) :
  m_commands(commands)
{
  const bool VARINT_PREFIXED = true;
  const uint_least8_t MARKER = 0xC0;
  this->m_receiveCommandLedPin = receiveCommandLedPin;
  this->m_replyCommandLedPin = replyCommandLedPin;
  m_messageProcessor = new ProtobufMessagesProcessor<theodolite_messages_CommandEnvelope>(
  interface, VARINT_PREFIXED, MARKER, theodolite_messages_CommandEnvelope_fields);
}

bool ProtocolProcessor::sendState(uint32_t requestId)
{
  theodolite_messages_CommandEnvelope envelope =
  {0};
  envelope.request_id = requestId;

  SystemState sysState = m_commands.getState();
  envelope.has_state = true;
  envelope.state.turner_position.azimuth = considerAnglePeriod(
                                                               sysState.azimuthMills,
                                                               sysState.deltaAzimuth);
  envelope.state.turner_position.elevation = considerAnglePeriod(
                                                                 sysState.elevationMills,
                                                                 sysState.deltaElevation);
  envelope.state.blocking_mode.is_azimuth_blocked = sysState.azimuthBrake;
  envelope.state.blocking_mode.is_elevation_blocked = sysState.elevationBrake;
  envelope.state.blocking_mode.is_auto_blocking_mode = sysState.autoBlockMode;
  envelope.state.azimuth_drive_activity = sysState.azimuthDriveActivity;
  envelope.state.elevation_drive_activity = sysState.elevationDriveActivity;
  envelope.state.state = sysState.state;

  envelope.state.inclinometer_parameters.x = sysState.inclinometerXMills;
  envelope.state.inclinometer_parameters.y = sysState.inclinometerYMills;
  if (sysState.frequency != CameraFreq_FREQ_UNKNOWN)
  {
    envelope.state.camera_info[0].camera_settings.exposure = sysState.exposure;
    envelope.state.camera_info[0].camera_settings.gain = sysState.gain;
    envelope.state.camera_info[0].camera_settings.exposure_type =
    getCameraExpositionTypeEnum(sysState.cameraExpositionType);
    envelope.state.camera_info[0].camera_settings.has_exposure_type =
    true;
    envelope.state.camera_info[0].camera_settings.freq = getCameraFreqEnum(
                                                                           sysState.frequency);
    envelope.state.camera_info[0].camera_settings.has_freq = true;
    envelope.state.camera_info[0].camera_settings.sync_type =
    getCameraSyncTypeEnum(sysState.cameraSyncType);
    envelope.state.camera_info[0].camera_settings.has_sync_type =
    true;
    envelope.state.camera_info_count = 1;
  }

  PidRegulatorInterface &azimuthReg =
  m_commands.getAzimuthMoveProcessor().getRegulator();
  envelope.state.regulator_coeff.has_azimuth_regulator = true;
  envelope.state.regulator_coeff.azimuth_regulator.proportional_gain =
  azimuthReg.getKp();
  envelope.state.regulator_coeff.azimuth_regulator.integral_correction_gain =
  azimuthReg.getKc();
  envelope.state.regulator_coeff.azimuth_regulator.integral_gain =
  azimuthReg.getKi();
  envelope.state.regulator_coeff.azimuth_regulator.differential_gain = azimuthReg.getKd();
  PidRegulatorInterface &elevationReg =
  m_commands.getElevationMoveProcessor().getRegulator();
  envelope.state.has_regulator_coeff = true;
  envelope.state.regulator_coeff.has_elevation_regulator = true;
  envelope.state.regulator_coeff.elevation_regulator.proportional_gain =
  elevationReg.getKp();
  envelope.state.has_angles_delta = true;
  envelope.state.angles_delta.azimuth = sysState.deltaAzimuth;
  envelope.state.angles_delta.elevation = sysState.deltaElevation;
  envelope.state.regulator_coeff.elevation_regulator.integral_correction_gain =
  elevationReg.getKc();
  envelope.state.regulator_coeff.elevation_regulator.integral_gain =
  elevationReg.getKi();
  envelope.state.regulator_coeff.elevation_regulator.differential_gain = elevationReg.getKd();
  envelope.state.has_encoder_board_state = true;

  envelope.state.encoder_board_state.azimuth_position = m_commands.getAzimuthMoveProcessor().getAxisPosition();
  envelope.state.encoder_board_state.elevation_position = m_commands.getElevationMoveProcessor().getAxisPosition();

  if (m_commands.getTestingQueue() != NULL && m_commands.getTestingQueue()->getCount())
  {
    envelope.state.has_testing = true;
    Test test = m_commands.getTestingQueue()->get();
    envelope.state.testing.test = getTestEnum(test.test);
    if (test.isRunning == false)
    {
      envelope.state.testing.has_is_passed = true;
      envelope.state.testing.is_passed = test.isPassed;
    }
  }
  envelope.state.errors_count = 0;
  while (errorsStorage.getErrorCount())
  {
    theodolite_messages_Error error = errorsStorage.getError();
    envelope.state.errors[envelope.state.errors_count].device = error.device;
    envelope.state.errors[envelope.state.errors_count].error_count = 1;
    envelope.state.errors[envelope.state.errors_count].error[0] = error.error[0];
    ++envelope.state.errors_count;
  }

  size_t sizeOfEventBuffer = sizeof(envelope.state.manual_controller_events)
  / sizeof(theodolite_messages_ManualControllerEvent);
  Queue<ControllersEvent> *manualControllerEventQueue = m_commands.getManualControllerEventQueue();
  if (manualControllerEventQueue != NULL)
  {
    for (uint32_t i = 0; manualControllerEventQueue->getCount() && (i < sizeOfEventBuffer); i++)
    {
      envelope.state.manual_controller_events[i] = getControllerEventEnum(
                                                                          manualControllerEventQueue->get());
      envelope.state.manual_controller_events_count = i + 1;
    }
  }
  m_replyCommandLedPin->setLevel(!m_replyCommandLedPin->getLevel());
  return m_messageProcessor->sendMessage(envelope);
}

uint32_t ProtocolProcessor::considerAnglePeriod(uint32_t milliseconds)
{
  return milliseconds % MILLISEC_360_DEGREES;
}

uint32_t ProtocolProcessor::considerAnglePeriod(uint32_t milliseconds, uint32_t delta)
{
  uint32_t result = milliseconds % MILLISEC_360_DEGREES;
  delta = delta % MILLISEC_360_DEGREES;
  if (result < delta)
  {
    result = MILLISEC_360_DEGREES - (delta - result);
  }
  else
  {
    result -= delta;
  }
  return result;
}

bool ProtocolProcessor::sendState()
{
  return sendState(0);
}

bool ProtocolProcessor::parseIncomingMessage()
{
  theodolite_messages_CommandEnvelope message =
  {0};
  if (m_messageProcessor->fillMessageFromUartLink(message) == true)
  {
    m_receiveCommandLedPin->setLevel(!m_receiveCommandLedPin->getLevel());
    if (message.has_set_angles_delta)
    {
      m_commands.setGeoDelta(message.set_angles_delta.delta.azimuth,  message.set_angles_delta.delta.elevation);
      return sendAck(message.request_id);
    }
    else if (message.has_rotate_to_coordinates_req)
    {
      bool blockingMode = false;
      if (message.rotate_to_coordinates_req.has_is_auto_blocking_mode)
      {
        blockingMode = message.rotate_to_coordinates_req.is_auto_blocking_mode;
      }
      m_commands.rotateToCoordinatesInMills(
      considerAnglePeriod(message.rotate_to_coordinates_req.turner_position.azimuth),
      considerAnglePeriod(message.rotate_to_coordinates_req.turner_position.elevation),
      blockingMode);
      return sendAck(message.request_id);
    }
    else if (message.has_adjust_shooting_settings_req)
    {
      m_commands.setCameraExposure(
                                   message.adjust_shooting_settings_req.camera_settings.exposure);
      m_commands.setCameraGain(
                               message.adjust_shooting_settings_req.camera_settings.gain);
      if (message.adjust_shooting_settings_req.camera_settings.has_exposure_type)
      {
        CameraExpositionType expo =
        getCameraExpositionTypeEnum(
                                    message.adjust_shooting_settings_req.camera_settings.exposure_type);
        m_commands.setCameraExpoType(expo);
      }
      if (message.adjust_shooting_settings_req.camera_settings.has_sync_type)
      {
        CameraSyncType sync =
        getCameraSyncTypeEnum(
                              message.adjust_shooting_settings_req.camera_settings.sync_type);
        m_commands.setCameraSyncType(sync);
      }
      if (message.adjust_shooting_settings_req.camera_settings.has_freq)
      {
        CameraFreq freq =
        getCameraFreqEnum(
                          message.adjust_shooting_settings_req.camera_settings.freq);
        m_commands.setCameraFrequency(freq);
      }
      m_commands.applyCameraChanges();
      return sendAck(message.request_id);
    }
    else if (message.has_state_req)
    {
      return sendState(message.request_id);
    }
    else if (message.has_set_regulator_coeff)
    {
      if (message.set_regulator_coeff.regulator_info.has_azimuth_regulator)
      {
        m_commands.setAzimuthPidCoeff(message.set_regulator_coeff.regulator_info.azimuth_regulator);
      }
      if (message.set_regulator_coeff.regulator_info.has_elevation_regulator)
      {
        m_commands.setElevationPidCoeff(message.set_regulator_coeff.regulator_info.elevation_regulator);
      }
      return sendAck(message.request_id);
    }
    else if (message.has_test_req)
    {
      m_commands.setTestMode();
      return sendAck(message.request_id);
    }
    else
    {
      return sendNak(message.request_id);
    }
  }
  return false;
}

CameraSyncType ProtocolProcessor::getCameraSyncTypeEnum(
                                                        theodolite_messages_SynchronizationType syncType)
{
  CameraSyncType retVal;
  retVal = static_cast<CameraSyncType>(syncType);
  return retVal;
}

theodolite_messages_SynchronizationType ProtocolProcessor::getCameraSyncTypeEnum(
                                                                                 CameraSyncType syncType)
{
  theodolite_messages_SynchronizationType retVal;
  retVal = static_cast<theodolite_messages_SynchronizationType>(syncType);
  return retVal;
}

theodolite_messages_ExposureType ProtocolProcessor::getCameraExpositionTypeEnum(
                                                                                CameraExpositionType expoType)
{
  theodolite_messages_ExposureType retVal;
  retVal = static_cast<theodolite_messages_ExposureType>(expoType);
  return retVal;
}

CameraExpositionType ProtocolProcessor::getCameraExpositionTypeEnum(
                                                                    theodolite_messages_ExposureType expoType)
{
  CameraExpositionType retVal;
  retVal = static_cast<CameraExpositionType>(expoType);
  return retVal;
}

theodolite_messages_Test ProtocolProcessor::getTestEnum(TestType test)
{
  theodolite_messages_Test retVal;
  retVal = static_cast<theodolite_messages_Test>(test);
  return retVal;
}

theodolite_messages_ManualControllerEvent ProtocolProcessor::getControllerEventEnum(ControllersEvent event)
{
  switch (event)
  {
    case AZIMUTH_INCREMENT_PRESSED:
      return theodolite_messages_ManualControllerEvent_AZIMUTH_INCREMENT_PRESSED_EVENT;
    case AZIMUTH_INCREMENT_RELEASED:
      return theodolite_messages_ManualControllerEvent_AZIMUTH_INCREMENT_RELEASED_EVENT;
    case AZIMUTH_DECREMENT_PRESSED:
      return theodolite_messages_ManualControllerEvent_AZIMUTH_DECREMENT_PRESSED_EVENT;
    case AZIMUTH_DECREMENT_RELEASED:
      return theodolite_messages_ManualControllerEvent_AZIMUTH_DECREMENT_RELEASED_EVENT;
    case ELEVATION_INCREMENT_PRESSED:
      return theodolite_messages_ManualControllerEvent_ELEVATION_INCREMENT_PRESSED_EVENT;
    case ELEVATION_INCREMENT_RELEASED:
      return theodolite_messages_ManualControllerEvent_ELEVATION_INCREMENT_RELEASED_EVENT;
    case ELEVATION_DECREMENT_PRESSED:
      return theodolite_messages_ManualControllerEvent_ELEVATION_DECREMENT_PRESSED_EVENT;
    case ELEVATION_DECREMENT_RELEASED:
      return theodolite_messages_ManualControllerEvent_ELEVATION_DECREMENT_RELEASED_EVENT;
    case CONTROLLER_CONNECT:
      return theodolite_messages_ManualControllerEvent_CONTROLLER_CONNECT_EVENT;
    case CONTROLLER_DISCONNECT:
      return theodolite_messages_ManualControllerEvent_CONTROLLER_DISCONNECT_EVENT;
  }
  return theodolite_messages_ManualControllerEvent_CONTROLLER_DISCONNECT_EVENT; //TODO подумать, как обрабатывать ошибки
}

timesync_messages_Frequency ProtocolProcessor::getCameraFreqEnum(
                                                                 CameraFreq freq)
{
  timesync_messages_Frequency frequency;
  switch (freq)
  {
    case CameraFreq_FREQ_16:
      frequency = timesync_messages_Frequency_HERTZ_16;
      break;
    case CameraFreq_FREQ_8:
      frequency = timesync_messages_Frequency_HERTZ_8;
      break;
    case CameraFreq_FREQ_4:
      frequency = timesync_messages_Frequency_HERTZ_4;
      break;
    case CameraFreq_FREQ_2:
      frequency = timesync_messages_Frequency_HERTZ_2;
      break;
    case CameraFreq_FREQ_1:
      frequency = timesync_messages_Frequency_HERTZ_1;
      break;
    case CameraFreq_FREQ_0_5:
      frequency = timesync_messages_Frequency_HERTZ_0_5;
      break;
    default:
      frequency = timesync_messages_Frequency_HERTZ_1;
      break;
  }
  return frequency;
}

CameraFreq ProtocolProcessor::getCameraFreqEnum(
                                                timesync_messages_Frequency freq)
{
  CameraFreq frequency;
  switch (freq)
  {
    case timesync_messages_Frequency_HERTZ_16:
      frequency = CameraFreq_FREQ_16;
      break;
    case timesync_messages_Frequency_HERTZ_8:
      frequency = CameraFreq_FREQ_8;
      break;
    case timesync_messages_Frequency_HERTZ_4:
      frequency = CameraFreq_FREQ_4;
      break;
    case timesync_messages_Frequency_HERTZ_2:
      frequency = CameraFreq_FREQ_2;
      break;
    case timesync_messages_Frequency_HERTZ_1:
      frequency = CameraFreq_FREQ_1;
      break;
    case timesync_messages_Frequency_HERTZ_0_5:
      frequency = CameraFreq_FREQ_0_5;
      break;
    default:
      frequency = CameraFreq_FREQ_1;
      break;
  }
  return frequency;
}

