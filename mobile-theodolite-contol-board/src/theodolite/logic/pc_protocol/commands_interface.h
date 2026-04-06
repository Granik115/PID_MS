/*
 * commands_interface.h
 *
 *      Author: mmalenko
 * (c) 2016 NIC SPbETU
 */

#ifndef LOGIC_PC_PROTOCOL_COMMANDS_INTERFACE_H_
#define LOGIC_PC_PROTOCOL_COMMANDS_INTERFACE_H_

#include <theodolite/logic/camera_protocol/camera.h>
#include <theodolite/logic/axis_mover/axis_mover.h>
#include <theodolite/logic/pc_protocol/third-party/theodolite_messages.pb.h>
#include <firmware/queue/queue.h>

typedef enum
{
  AZIMUTH_MOVE_TEST = 1, //Тест драйвера азимута и обратной связи по току
  ELAVATION_MOVE_TEST = 2, //Тест драйвера угла места и обратной связи по току
  CAMERA_UART_KEY_TEST = 3, //Тест ключа управления камерой
  ENCODER_LOOPBACK_TEST = 4, //Тест приемопередачи интерфейса обмена с формирователем кода
  ONBOARD_FLASH_TEST = 5, //Тест записи/чтения микросхемы flash на плате
  ONBOARD_EEPROM_TEST = 6, //Тест записи/чтения микросхемы eeprom на плате
  INCOMING_SYNCHROIMPULSE_TEST = 7 //Тест приема внешнего синхроимпульса
} TestType;

typedef struct
{
  TestType test;
  bool isPassed;
  bool isRunning;
} Test;

typedef struct
{
  uint32_t azimuthMills;
  uint32_t elevationMills;
  uint32_t deltaAzimuth;
  uint32_t deltaElevation;
  int32_t inclinometerXMills;
  int32_t inclinometerYMills;
  uint32_t azimuthEncoderValues;
  uint32_t elevationEncoderValues;
  uint32_t inclinometerXADCValues;
  uint32_t inclinometerYADCValues;
  bool azimuthDriveActivity;
  bool elevationDriveActivity;
  bool azimuthBrake;
  bool elevationBrake;
  bool autoBlockMode;
  bool manualControllerConnected;
  theodolite_messages_StateType state;
  uint32_t gain;
  CameraFreq frequency;
  CameraExpositionType cameraExpositionType;
  CameraSyncType cameraSyncType;
  uint32_t exposure;
} SystemState;

class CommandsInterface
{
public:
  virtual void rotateToCoordinatesInMills(uint32_t azimuth, uint32_t elevation, bool blockingMode)=0;
  virtual void rotateToCoordinatesInEncoderValues(uint32_t azimuth, uint32_t elevation, bool blockingMode)=0;
  virtual void setGeoDelta(uint32_t azimuth, uint32_t elevation)=0;
  virtual void setCameraExposure(uint32_t exposure)=0;
  virtual SystemState getState()=0;
  virtual void applyCameraChanges()=0;
  virtual bool sendCameraRawCommand(uint_least8_t *buffer, uint16_t len)=0;
  virtual void setCameraGain(uint32_t gain)=0;
  virtual void setCameraFrequency(CameraFreq frequency)=0;
  virtual void setCameraExpoType(CameraExpositionType expoType)=0;
  virtual void setCameraSyncType(CameraSyncType syncType)=0;
  virtual bool saveState()=0;
  virtual void setAzimuthPidCoeff(theodolite_messages_PidRegulatorCoeff)=0;
  virtual void setElevationPidCoeff(theodolite_messages_PidRegulatorCoeff)=0;
  virtual axis_mover_interface::AxisMoverInterface& getAzimuthMoveProcessor()=0;
  virtual axis_mover_interface::AxisMoverInterface& getElevationMoveProcessor()=0;
  virtual Queue<ControllersEvent> *getManualControllerEventQueue()=0;
  virtual Queue<Test> *getTestingQueue()=0;
  virtual void setTestMode()=0;
  virtual void updateTest()=0;
  virtual ~CommandsInterface()
  {

  }
};

#endif /* LOGIC_PC_PROTOCOL_COMMANDS_INTERFACE_H_ */
