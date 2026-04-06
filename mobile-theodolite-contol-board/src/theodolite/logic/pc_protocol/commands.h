/*
 * commands.h
 *
 *      Author: mmalenko
 * (c) 2016 NIC SPbETU
 */

#ifndef LOGIC_PC_PROTOCOL_COMMANDS_H_
#define LOGIC_PC_PROTOCOL_COMMANDS_H_

#include "commands_interface.h"
#include <theodolite/logic/move_controller/move_controller_interface.h>
#include <theodolite/logic/manual_controller/manual_controller_interface.h>
#include <theodolite/logic/memory/memory_interface.h>
#include <firmware/queue/queue.h>
#include <theodolite/logic/memory/state_saver_interface.h>
#include <theodolite/logic/sync_timer/sync_timer_interface.h>
#include <theodolite/logic/sys_config/sys_tester_interface.h>

class Commands:
  public CommandsInterface
{
  SystemTesterInterface *m_tester;
  TestType m_currentTest;
  Queue<Test> m_testQueue;
  axis_mover_interface::StateType m_state;
  MoveControllerInterface &m_moveController;
  ManualControllerInterface &m_manualController;
  CameraInterface *m_camera;
  StateSaverInterface &m_stateSaver;
  uint32_t m_deltaAzimuth;
  uint32_t m_deltaElevation;
  static const int MSEC_TO_ENCODER_COEFF = 1800;
  static void setUpRegulator(PidRegulatorInterface &regulator, float kp, float ki, float kd,
                             float kc);
  static CameraGain getCameraGainEnum(uint32_t);
  static uint32_t getCameraGainValue(CameraGain gain);
  void sendTestRunning();
  void sendTestFinished(bool isPassed);
  void setAzimuthPidCoeff(float kp, float ki, float kd, float kc);
  void setElevationPidCoeff(float kp, float ki, float kd, float kc);
  void fillPidStructure(theodolite_messages_PidRegulatorCoeff&, theodolite_messages_PidRegulatorCoeff&);
  bool comparePidStructure(theodolite_messages_PidRegulatorCoeff&, theodolite_messages_PidRegulatorCoeff&);
  public:
  bool saveState();
  bool sendCameraRawCommand(uint_least8_t *buffer, uint16_t len);
  void rotateToCoordinatesInMills(uint32_t azimuth, uint32_t elevation, bool blockingMode);
  void setGeoDelta(uint32_t azimuth, uint32_t elevation);
  void rotateToCoordinatesInEncoderValues(uint32_t azimuth, uint32_t elevation, bool blockingMode);
  void setCameraExposure(uint32_t exposure);
  SystemState getState();
  void applyCameraChanges();
  void setCameraGain(uint32_t gain);
  void setCameraFrequency(CameraFreq frequency);
  void setCameraExpoType(CameraExpositionType expoType);
  void setCameraSyncType(CameraSyncType syncType);
  void setAzimuthPidCoeff(theodolite_messages_PidRegulatorCoeff);
  void setElevationPidCoeff(theodolite_messages_PidRegulatorCoeff);
  axis_mover_interface::AxisMoverInterface& getAzimuthMoveProcessor();
  axis_mover_interface::AxisMoverInterface& getElevationMoveProcessor();
  Queue<ControllersEvent>* getManualControllerEventQueue();
  Queue<Test>* getTestingQueue();
  void setTestMode();
  void updateTest();
  Commands(MoveControllerInterface &moveController, CameraInterface *camera,
           ManualControllerInterface &manualController,
           SystemTesterInterface *tester, StateSaverInterface &pidSaver);
  virtual ~Commands()
  {

  }
};

#endif /* LOGIC_PC_PROTOCOL_COMMANDS_H_ */
