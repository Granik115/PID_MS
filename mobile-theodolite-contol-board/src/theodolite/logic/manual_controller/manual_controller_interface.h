/*
 * manual_controller_interface.h
 *
 *  Created on: 24 янв. 2017 г.
 *      Author: mmalenko
 *  (c) 2017 NIC SPbETU
 */

#ifndef LOGIC_MANUAL_CONTROLLER_MANUAL_CONTROLLER_INTERFACE_H_
#define LOGIC_MANUAL_CONTROLLER_MANUAL_CONTROLLER_INTERFACE_H_

#include <theodolite/logic/external_controls/controls.h>
#include <firmware/queue/queue.h>

class ManualControllerInterface
{
public:
  virtual bool isControllerConnected()=0;
  virtual Queue<ControllersEvent> *getManualControllerEventQueue()=0;
  virtual ~ManualControllerInterface()
  {

  }
  virtual void update()=0;
};

#endif /* LOGIC_MANUAL_CONTROLLER_MANUAL_CONTROLLER_INTERFACE_H_ */
