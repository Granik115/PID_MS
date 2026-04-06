///*
// * pwm.cpp
// *
// *      Author: mmalenko
// * (c) 2017 NIC SPbETU
// */

#include "stddef.h"
#include <theodolite/logic/axis_mover/axis_mover_interface.h>
#include <theodolite/logic/local_clock/local_clock.h>

void periodicIsrCall(void);

#ifdef FREEMASTER_PROTOCOL
extern void updateRecorder();
#endif

axis_mover_interface::AxisMoverInterface *azimuthMoveProcessor = NULL;
axis_mover_interface::AxisMoverInterface *elevationMoveProcessor = NULL;
EncoderBoardInterface *encoderBoard = NULL;
PwmInterface *azimuthMotor = NULL;
PwmInterface *elevationMotor = NULL;

void periodicIsrCall(void)
{
  localClockTimerISR();
  encoderBoard->updateWithoutRequest();
  azimuthMoveProcessor->updateWithoutRequest();
  elevationMoveProcessor->updateWithoutRequest();
  elevationMotor->updateWithoutRequest();
  azimuthMotor->updateWithoutRequest();
#ifdef FREEMASTER_PROTOCOL
  updateRecorder();
#endif
}

void setMotors(PwmInterface *azimuth, PwmInterface *elevation)
{
  azimuthMotor = azimuth;
  elevationMotor = elevation;
}

void setEncoderBoard(EncoderBoardInterface *encoder)
{
  encoderBoard = encoder;
}

void setMoveProcessors(
                       axis_mover_interface::AxisMoverInterface *azimuthProcessor,
                       axis_mover_interface::AxisMoverInterface* elevationProcessor)
{
  azimuthMoveProcessor = azimuthProcessor;
  elevationMoveProcessor = elevationProcessor;
}
