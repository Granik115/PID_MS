/*
 * protocol_task.cpp
 *
 *  Created on: Jan 9, 2025
 *      Author: mvm
 *   (c) 2025 NIC SPbETU
 */

#include "cmsis_os2.h"
#include <theodolite/logic/pc_protocol/protocol_processor.h>
#include <theodolite/logic/pc_protocol/mkti_commands.h>
#include "theodolite/logic/move_controller/move_controller.h"
#include "tcp_server/tcp_connection_interface.h"
#include "tcp_server/tcp_server.h"
#include "manual_controller/manual_controller_dummy.h"
#include "memory/state_saver.h"
#include "encoder_board/mkti_sensors_agregator.h"
#include "axis_mover/axis_mover_foc.h"
#include "pwm/motor_pwm.h"
#include "FreeRTOS.h"
#include "task.h"
#include "encoder_board/digital_inclinometer/ind3.h"
#include "main.h"
#include "foc/foc.h"
#include "USBSerial.h"

static void protocolTask(void *argument);
static osThreadId_t defaultTaskHandle;
static const osThreadAttr_t defaultTask_attributes =
{
  .name = "protocol_task",
  .stack_size = 7000,
  .priority = (osPriority_t)osPriorityNormal,
};

static osMessageQueueId_t poolSocketsQueue = osMessageQueueNew(5, 5, NULL);

bool procotolTaskInit()
{
  defaultTaskHandle = osThreadNew(protocolTask, NULL, &defaultTask_attributes);
  initTcpServers();
  return true;
}

extern TcpConnection connectionsPool[SOCKETS_MAX_COUNT];
volatile size_t freeSize = 0;

static void protocolTask(void *argument)
{
  PidRegulator pidDummy = PidRegulator(0, 0, 0, 0, 0, 0);
  ConnectionInterfaceDummy dummyInterface;
  volatile UBaseType_t uxHighWaterMark;
  PB_UNUSED(uxHighWaterMark);
  uxHighWaterMark = uxTaskGetStackHighWaterMark((TaskHandle_t)defaultTaskHandle);
  MKTISensorsAgregator sensorsAgregator = MKTISensorsAgregator(dummyInterface, getAzimuthEncoder(),
                                                               getElevationEncoder());
  Brake azimuthBrake = Brake(getPin(AZIMUTH_BRAKE_PIN));
  Brake elevationBrake = Brake(getPin(ELEVATION_BRAKE_PIN));
  FOCAzimuthAxisMover focAzimuthAxisMover = FOCAzimuthAxisMover(
                                                                sensorsAgregator,
                                                                pidDummy,
                                                                azimuthBrake,
                                                                getAzimuthMotor());
  MockAxisMover focElevationAxisMover = MockAxisMover(
                                                      sensorsAgregator,
                                                      pidDummy,
                                                      elevationBrake,
                                                      getElevationMotor(),
                                                      getElevationEncoder());

  MoveController moveController = MoveController(focAzimuthAxisMover, focElevationAxisMover);
  ManualControllerDummy manualController;
  StateSaver stateSaver = StateSaver(getInternalEeprom());
  Camera camera = Camera(dummyInterface);
  MktiCommands commands = MktiCommands(moveController, &camera, manualController, stateSaver);
  Data<ConnectionInterface> connections = getTcpConnectionsPool();
  ProtocolProcessorInterface *protocolProcessors[connections.m_size] = {};
  for (uint32_t i = 0; i < connections.m_size; i++)
    protocolProcessors[i] = getProcessorInterface(MODERN_PROTOCOL, commands,
                                                  connectionsPool[i],
                                                  getPin(RECEIVE_COMMAND_LED_PIN),
                                                  getPin(SEND_REPLY_LED_PIN));
  Timer sendDataTimer = Timer(100);
  for (;;)
  {
    if (sendDataTimer.expired())
    {
      for (uint8_t i = 0; i < connections.m_size; i++)
      {
        if (connectionsPool[i].isPeripheralOk())
          protocolProcessors[i]->sendState();
      }
      sendDataTimer.restart();
      sensorsAgregator.updateWithoutRequest();
      LL_GPIO_TogglePin(TEST_LED_GPIO_Port, TEST_LED_Pin);
    }
    for (uint8_t i = 0; i < connections.m_size; i++)
    {
      if (connectionsPool[i].isPeripheralOk())
        protocolProcessors[i]->parseIncomingMessage();
    }
    freeSize = xPortGetFreeHeapSize();
    focAzimuthAxisMover.updateWithoutRequest();
   // SerialUSB.printf("azimuth state: %d, brake state: %d\n", (int)focAzimuthAxisMover.getState(),
   //                  (int)azimuthBrake.getBrakeState());
    osDelay(50); //osSemaphore for incoming tcp data;
  }
}

