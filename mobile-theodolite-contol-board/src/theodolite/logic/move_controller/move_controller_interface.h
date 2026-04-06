/*
 * move_controller_interface.h
 *
 *  Created on: 28 июн. 2017 г.
 *      Author: mmalenko
 *
 * (c) 2017 NIC SPbETU
 */

#ifndef SRC_THEODOLITE_LOGIC_MOVE_CONTROLLER_MOVE_CONTROLLER_INTERFACE_H_
#define SRC_THEODOLITE_LOGIC_MOVE_CONTROLLER_MOVE_CONTROLLER_INTERFACE_H_

#include <theodolite/logic/pwm/pwm_interface.h>
#include <theodolite/logic/axis_mover/axis_mover_interface.h>

class MoveControllerInterface
{
public:
  virtual ~MoveControllerInterface()
  {

  }
  virtual void setExplicitAngle(uint32_t azimuth, uint32_t elevation, bool blockingMode)=0;
  virtual void rotateAzimuthAxisManualMode(axis_mover_interface::AxisMoveDirection)=0;
  virtual void rotateElevationAxisManualMode(axis_mover_interface::AxisMoveDirection)=0;
  virtual void stopAzimuthAxisManualMode()=0;
  virtual void stopElevationAxisManualMode()=0;
  virtual void releaseAzimuthAxis()=0;
  virtual void releaseElevationAxis()=0;
  virtual void setManualMode(bool)=0;
  virtual axis_mover_interface::AxisMoverInterface& getAzimuthAxisMover()=0;
  virtual axis_mover_interface::AxisMoverInterface& getElevationAxisMover()=0;
};

#endif /* SRC_THEODOLITE_LOGIC_MOVE_CONTROLLER_MOVE_CONTROLLER_INTERFACE_H_ */
