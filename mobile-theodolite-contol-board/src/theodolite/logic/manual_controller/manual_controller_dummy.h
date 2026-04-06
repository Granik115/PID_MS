/*
 * manual_controller_dummy.h
 *
 *  Created on: Jan 10, 2025
 *      Author: mvm
 */

#ifndef THEODOLITE_LOGIC_MANUAL_CONTROLLER_MANUAL_CONTROLLER_DUMMY_H_
#define THEODOLITE_LOGIC_MANUAL_CONTROLLER_MANUAL_CONTROLLER_DUMMY_H_

#include <theodolite/logic/external_controls/controls.h>
#include "theodolite/logic/manual_controller/manual_controller_interface.h"
#include "theodolite/logic/move_controller/move_controller_interface.h"

class ManualControllerDummy:
  public ManualControllerInterface
{
public:
  bool isControllerConnected()
  {
    return false;
  }
  ManualControllerDummy()
  {

  }
  void update()
  {

  }
  Queue<ControllersEvent>* getManualControllerEventQueue()
  {
    return NULL;
  }
};

#endif /* THEODOLITE_LOGIC_MANUAL_CONTROLLER_MANUAL_CONTROLLER_DUMMY_H_ */
