/*
 * mkti_commands.h
 *
 *  Created on: Jan 9, 2025
 *      Author: mvm
 */

#ifndef THEODOLITE_LOGIC_PC_PROTOCOL_MKTI_COMMANDS_H_
#define THEODOLITE_LOGIC_PC_PROTOCOL_MKTI_COMMANDS_H_

#include "commands_interface.h"
#include <theodolite/logic/move_controller/move_controller_interface.h>
#include <theodolite/logic/manual_controller/manual_controller_interface.h>
#include <theodolite/logic/memory/memory_interface.h>
#include <firmware/queue/queue.h>
#include <theodolite/logic/memory/state_saver_interface.h>
#include <theodolite/logic/sync_timer/sync_timer_interface.h>
#include <theodolite/logic/sys_config/sys_tester_interface.h>
#include <theodolite/logic/pc_protocol/commands.h>

class MktiCommands:
  public Commands
{
public:
  MktiCommands(MoveControllerInterface &moveController, CameraInterface *camera,
               ManualControllerInterface &manualController,
               StateSaverInterface &stateSaver) :
    Commands(moveController, camera, manualController, NULL, stateSaver)
  {

  }
  virtual ~MktiCommands()
  {

  }
};

#endif /* THEODOLITE_LOGIC_PC_PROTOCOL_MKTI_COMMANDS_H_ */
