/*
 * manual_controller.cpp
 *
 *  Created on: 24 янв. 2017 г.
 *      Author: mmalenko
 *  (c) 2017 NIC SPbETU
 */

#include "manual_controller.h"

const bool CONTROLLER_CONNECTION_LEVEL = false;

const ControllersEvent turn180Sequence[4] =
{AZIMUTH_INCREMENT_PRESSED,
  AZIMUTH_DECREMENT_PRESSED,
  AZIMUTH_INCREMENT_RELEASED,
  AZIMUTH_DECREMENT_RELEASED};
const ControllersEvent turn90Sequence[4] =
{AZIMUTH_DECREMENT_PRESSED,
  AZIMUTH_INCREMENT_PRESSED,
  AZIMUTH_DECREMENT_RELEASED,
  AZIMUTH_INCREMENT_RELEASED};

bool ManualController::isTurn180Combination(ControllersEvent event)
{
  if (event == turn180Sequence[turn180SequenceCounter])
  {
    ++turn180SequenceCounter;
    if (turn180SequenceCounter == sizeof(turn180Sequence))
    {
      return true;
    }
  }
  return false;
}

bool ManualController::isTurn90Combination(ControllersEvent event)
{
  if (event == turn90Sequence[turn90SequenceCounter])
  {
    ++turn90SequenceCounter;
    if (turn90SequenceCounter == sizeof(turn90Sequence))
    {
      return true;
    }
  }
  return false;
}

void ManualController::update()
{
  controllerSense.update();
  azimuthUpButton.update();
  azimuthDownButton.update();
  elevationUpButton.update();
  elevationDownButton.update();
  if (ButtonAbstract::isEventGenerated())
  {
    const ControllersEvent event = ButtonAbstract::getEvent();
    eventQueue.put(event);
    switch (event)
    {
      case CONTROLLER_CONNECT:
        controllerConnected = true;
        moveController.setManualMode(true);
        moveController.stopAzimuthAxisManualMode();
        moveController.stopElevationAxisManualMode();
        break;
      case CONTROLLER_DISCONNECT:
        controllerConnected = false;
        moveController.setManualMode(false);
        moveController.releaseAzimuthAxis();
        moveController.releaseElevationAxis();
        break;
      case AZIMUTH_INCREMENT_PRESSED:
        if (controllerConnected)
        {
          moveController.rotateAzimuthAxisManualMode(axis_mover_interface::AXIS_INCREMENT_DIRECTION);
        }
        break;
      case AZIMUTH_INCREMENT_RELEASED:
        case AZIMUTH_DECREMENT_RELEASED:
        if (controllerConnected)
        {
          moveController.stopAzimuthAxisManualMode();
        }
        break;
      case AZIMUTH_DECREMENT_PRESSED:
        if (controllerConnected)
        {
          moveController.rotateAzimuthAxisManualMode(axis_mover_interface::AXIS_DECREMENT_DIRECTION);
        }
        break;
      case ELEVATION_INCREMENT_PRESSED:
        if (controllerConnected)
        {
          moveController.rotateElevationAxisManualMode(axis_mover_interface::AXIS_INCREMENT_DIRECTION);
        }
        break;
      case ELEVATION_INCREMENT_RELEASED:
        case ELEVATION_DECREMENT_RELEASED:
        if (controllerConnected)
        {
          moveController.stopElevationAxisManualMode();
        }
        break;
      case ELEVATION_DECREMENT_PRESSED:
        if (controllerConnected)
        {
          moveController.rotateElevationAxisManualMode(axis_mover_interface::AXIS_DECREMENT_DIRECTION);
        }
        break;
      default:
        break;
    }
  }
}

ManualController::ManualController(MoveControllerInterface &moveController,
                                   ButtonAbstract &azimuthUpButton,
                                   ButtonAbstract &azimuthDownButton,
                                   ButtonAbstract &elevationUpButton,
                                   ButtonAbstract &elevationDownButton,
                                   ButtonAbstract &controllerSense) :
  eventQueue(5),
  moveController(moveController), azimuthUpButton(azimuthUpButton), azimuthDownButton(
                                                                                      azimuthDownButton),
  elevationUpButton(elevationUpButton), elevationDownButton(
                                                            elevationDownButton), controllerSense(controllerSense)
{
  controllerConnected = false;
  turn90SequenceCounter = 0;
  turn180SequenceCounter = 0;
}

Queue<ControllersEvent> *ManualController::getManualControllerEventQueue()
{
  return &eventQueue;
}

bool ManualController::isControllerConnected()
{
  return controllerConnected;
}

