/*
 * move_controller.h
 *
 *  Created on: 28 июн. 2017 г.
 *      Author: mmalenko
 *  (c) 2017 NIC SPbETU
 */

#ifndef SRC_THEODOLITE_LOGIC_MOVE_CONTROLLER_MOVE_CONTROLLER_H_
#define SRC_THEODOLITE_LOGIC_MOVE_CONTROLLER_MOVE_CONTROLLER_H_

#include <theodolite/logic/move_controller/move_controller_interface.h>

class MoveController:
  public MoveControllerInterface
{
  axis_mover_interface::AxisMoverInterface &azimuthAxisMover;
  axis_mover_interface::AxisMoverInterface &elevationAxisMover;
  bool ignoreAngle;
  void rotateAxisManualMode(axis_mover_interface::AxisMoveDirection, axis_mover_interface::AxisMoverInterface&);
  void stopAxisManualMode(axis_mover_interface::AxisMoverInterface&);
  public:
  MoveController(axis_mover_interface::AxisMoverInterface &azimuthProcessor,
                 axis_mover_interface::AxisMoverInterface &elevationProcessor);
  void setExplicitAngle(uint32_t azimuth, uint32_t elevation, bool blockingMode);
  void rotateAzimuthAxisManualMode(axis_mover_interface::AxisMoveDirection);
  void rotateElevationAxisManualMode(axis_mover_interface::AxisMoveDirection);
  void releaseAzimuthAxis();
  void releaseElevationAxis();
  void stopAzimuthAxisManualMode();
  void stopElevationAxisManualMode();
  void setManualMode(bool);
  axis_mover_interface::AxisMoverInterface&
  getAzimuthAxisMover();
  axis_mover_interface::AxisMoverInterface&
  getElevationAxisMover();
};

#endif /* SRC_THEODOLITE_LOGIC_MOVE_CONTROLLER_MOVE_CONTROLLER_H_ */
