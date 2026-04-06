/*
 * move_controller.cpp
 *
 *  Created on: 28 июн. 2017 г.
 *      Author: mmalenko
 *  (c) 2017 NIC SPbETU
 */

#include <theodolite/logic/move_controller/move_controller.h>
#include <theodolite/logic/sys_config/sys_cfg.h>

MoveController::MoveController(axis_mover_interface::AxisMoverInterface &azimuthAxisMover,
                               axis_mover_interface::AxisMoverInterface &elevationAxisMover) :
  azimuthAxisMover(azimuthAxisMover), elevationAxisMover(elevationAxisMover)
{
  ignoreAngle = false;
}

void MoveController::setExplicitAngle(uint32_t azimuth, uint32_t elevation, bool blockingMode)
{
  if (!ignoreAngle)
  {
    azimuthAxisMover.setExplicitAngleCode(azimuth, blockingMode);
    elevationAxisMover.setExplicitAngleCode(elevation, blockingMode);
  }
}

void MoveController::rotateAxisManualMode(axis_mover_interface::AxisMoveDirection direction,
                                          axis_mover_interface::AxisMoverInterface &mover)
{
  mover.rotate(direction);
}

void MoveController::stopAxisManualMode(axis_mover_interface::AxisMoverInterface &mover)
{
  mover.rotate(axis_mover_interface::AXIS_STOPED);
}

void MoveController::rotateAzimuthAxisManualMode(axis_mover_interface::AxisMoveDirection direction)
{
  rotateAxisManualMode(direction, azimuthAxisMover);
}
void MoveController::rotateElevationAxisManualMode(axis_mover_interface::AxisMoveDirection direction)
{
  rotateAxisManualMode(direction, elevationAxisMover);
}
void MoveController::stopAzimuthAxisManualMode()
{
  stopAxisManualMode(azimuthAxisMover);
}
void MoveController::stopElevationAxisManualMode()
{
  stopAxisManualMode(elevationAxisMover);
}

axis_mover_interface::AxisMoverInterface& MoveController::getAzimuthAxisMover()
{
  return azimuthAxisMover;
}
axis_mover_interface::AxisMoverInterface& MoveController::getElevationAxisMover()
{
  return elevationAxisMover;
}

void MoveController::setManualMode(bool mode)
{
  ignoreAngle = mode;
  if (!mode)
  {
    azimuthAxisMover.setMotorRampMode(RAMPCONTROL_LINEAR);
    elevationAxisMover.setMotorRampMode(RAMPCONTROL_LINEAR);
  }
}

void MoveController::releaseAzimuthAxis()
{
  azimuthAxisMover.release();

}
void MoveController::releaseElevationAxis()
{
  elevationAxisMover.release();
}
