/*
 * manual_controller.h
 *
 *  Created on: 24 янв. 2017 г.
 *      Author: mmalenko
 * (c) 2017 NIC SPbETU
 */

#ifndef LOGIC_MANUAL_CONTROLLER_MANUAL_CONTROLLER_H_
#define LOGIC_MANUAL_CONTROLLER_MANUAL_CONTROLLER_H_

#include <theodolite/logic/external_controls/controls.h>
#include "theodolite/logic/manual_controller/manual_controller_interface.h"
#include "theodolite/logic/move_controller/move_controller_interface.h"

class ManualController:
  public ManualControllerInterface
{
  Queue<ControllersEvent> eventQueue;
  MoveControllerInterface &moveController;
  bool controllerConnected;
  ButtonAbstract &azimuthUpButton;
  ButtonAbstract &azimuthDownButton;
  ButtonAbstract &elevationUpButton;
  ButtonAbstract &elevationDownButton;
  ButtonAbstract &controllerSense;
  uint16_t turn90SequenceCounter;
  uint16_t turn180SequenceCounter;
  bool isTurn180Combination(ControllersEvent event);
  bool isTurn90Combination(ControllersEvent event);
  public:
  bool isControllerConnected();
  ManualController(MoveControllerInterface &moveController, ButtonAbstract &azimuthUpButton,
                   ButtonAbstract &azimuthDownButton,
                   ButtonAbstract &elevationUpButton,
                   ButtonAbstract &elevationDownButton,
                   ButtonAbstract &controllerSense);
  void update();
  Queue<ControllersEvent> *getManualControllerEventQueue();
};

#endif /* LOGIC_MANUAL_CONTROLLER_MANUAL_CONTROLLER_H_ */
