/*
 * axis_mover_tester.h
 *
 *  Created on: 7 июл. 2017 г.
 *      Author: mmalenko
 * (c) 2017 NIC SPbETU
 */

#ifndef AXIS_MOVER_TESTER_H_
#define AXIS_MOVER_TESTER_H_

#include <theodolite/logic/axis_mover/axis_mover.h>
#include <theodolite/logic/interfaces/interfaces.h>

typedef enum
{
  INIT_TEST,
  BRAKES_ON,
  BRAKES_OFF,
  PWM_TESTING_CLOCKWISE,
  PWM_TESTING_COUNTERCLOCKWISE,
  PWM_TESTING_STOP,
  PWM_INTEGRATION_TESTING_MAXIMUM,
  PWM_INTEGRATION_TESTING_MINIMUM,
  INITIALIZATION_TESTING,
  WAIT_TO_STOP,
  IDLE_TEST,
  ROTATION_TEST,
  SPEED_MEASURE_TEST,
  STOP_TESTING
} TestingState;

class AxisMoverTester:
  public AxisMover
{
  uint32_t speedMeasures[10];
  Timer *measureTimer;
  Timer *anglesSendTimer;
  float duty;
  bool isAxisLocked();
  TestingState testCase;
  Interface &protocolInterface;
  public:
  AxisMoverTester(EncoderBoardInterface &encoderBoard, PidRegulatorInterface &regulator,
                  BrakesInterface &brake,
                  PwmInterface &motor, Interface &protocolInterface) :
    AxisMover(encoderBoard, regulator, brake, motor), protocolInterface(protocolInterface)
  {
    duty = 0;
    measureTimer = new Timer(3000);
    anglesSendTimer = new Timer(100);
    testCase = INIT_TEST;
  }
  void update();
};

class AzimuthAxisMoverTester:
  public AxisMoverTester
{
  int32_t getCurrentEncoderAngle()
  {
    return encoderBoard.getAzimuth();
  }
public:
  AzimuthAxisMoverTester(EncoderBoardInterface &encoderBoard, PidRegulatorInterface &regulator,
                         BrakesInterface &brake,
                         PwmInterface &motor, Interface &interface) :
    AxisMoverTester(encoderBoard, regulator, brake, motor, interface)
  {

  }
};

class ElevationAxisMoverTester:
  public AxisMoverTester
{
  int32_t getCurrentEncoderAngle()
  {
    return encoderBoard.getElevation();
  }
public:
  ElevationAxisMoverTester(EncoderBoardInterface &encoderBoard, PidRegulatorInterface &regulator,
                           BrakesInterface &brake,
                           PwmInterface &motor, Interface &interface) :
    AxisMoverTester(encoderBoard, regulator, brake, motor, interface)
  {

  }
};

#endif /* AXIS_MOVER_TESTER_H_ */
