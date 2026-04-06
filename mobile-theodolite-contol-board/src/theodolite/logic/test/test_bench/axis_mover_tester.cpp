/*
 * axis_mover_tester.cpp
 *
 *  Created on: 7 июл. 2017 г.
 *      Author: mmalenko
 *  * (c) 2017 NIC SPbETU
 */

#include <theodolite/logic/sys_config/sys_cfg.h>
#include "test/test_bench/axis_mover_tester.h"
#include "stdio.h"

const float MAX_DUTY_VALUE = 100;

char testString[][32] =
{"INIT_TEST",
  "BRAKES_ON",
  "BRAKES_OFF",
  "PWM_TESTING_CLOCKWISE",
  "PWM_TESTING_COUNTERCLOCKWISE",
  "PWM_TESTING_STOP",
  "PWM_INTEGRATION_TESTING_MAXIMUM",
  "PWM_INTEGRATION_TESTING_MINIMUM",
  "INITIALIZATION_TESTING",
  "WAIT_TO_STOP",
  "IDLE_TEST",
  "ROTATION_TEST",
  "SPEED_MEASURE_TEST",
  "STOP_TESTING"};

int32_t debugArray[1000];
int32_t debugCnt = 0;

bool AxisMoverTester::isAxisLocked()
{
  bool axisLockedPredicate = false;
  if (updateImmediateSpeed() <= getMinimumSpeed(motor.getTimerDuty()))
  {
    axisLockedPredicate = true;
  }
  return axisLockedPredicate;
}

float duty;
int32_t staticErrorDebug;

void AxisMoverTester::update()
{
  if (anglesSendTimer->expired())
  {

    anglesSendTimer->reset();
    char data[50] = {};
    uint32_t angle = getCurrentEncoderAngle();
    int size = snprintf(data, 50, "%lu %s\n", angle, testString[testCase]);
    Data dataStruct;
    dataStruct.data = (uint_least8_t*)data;
    dataStruct.size = size;
    protocolInterface.send(&dataStruct);
  }
  if (updateRequestFlag == true)
  {
    updateImmediateSpeed();
    updateRequestFlag = false;
    switch (testCase)
    {
      case INIT_TEST:
        speedMeasureTimer->set(1000);
        speedMeasureTimer->restart();
        motor.setDuty(0);
        motor.disableInertion();
        brake.deactivate();
        testCase = BRAKES_ON;
        break;
      case BRAKES_ON:
        if (speedMeasureTimer->expired())
        {
          speedMeasureTimer->restart();
          brake.activate();
          testCase = BRAKES_OFF;
        }
        break;
      case BRAKES_OFF:
        if (speedMeasureTimer->expired())
        {
          speedMeasureTimer->restart();
          brake.deactivate();
          testCase = PWM_TESTING_CLOCKWISE;
          speedMeasureTimer->set(500);
        }
        break;
      case PWM_TESTING_CLOCKWISE:
        if (speedMeasureTimer->expired())
        {
          speedMeasureTimer->restart();
          motor.setDuty(duty);
          if (duty >= 100)
          {
            testCase = PWM_TESTING_COUNTERCLOCKWISE;
            break;
          }
          duty += 10;
        }
        break;
      case PWM_TESTING_COUNTERCLOCKWISE:
        if (speedMeasureTimer->expired())
        {
          speedMeasureTimer->restart();

          motor.setDuty(duty);
          if (duty <= -100)
          {
            testCase = PWM_TESTING_STOP;
            break;
          }
          duty -= 10;
        }
        break;
      case PWM_TESTING_STOP:
        if (speedMeasureTimer->expired())
        {
          speedMeasureTimer->restart();
          motor.setDuty(duty);
          if (duty >= 0)
          {
            testCase = PWM_INTEGRATION_TESTING_MAXIMUM;
          }
          duty += 10;
        }
        break;
      case PWM_INTEGRATION_TESTING_MAXIMUM:
        if (speedMeasureTimer->expired())
        {
          motor.enableInertion();
          motor.setInertionTimeCoeff(1);
          speedMeasureTimer->restart();
          motor.setDuty(100);
          if (motor.getDuty() == 100)
          {
            testCase = PWM_INTEGRATION_TESTING_MINIMUM;
          }
        }
        break;
      case PWM_INTEGRATION_TESTING_MINIMUM:
        if (speedMeasureTimer->expired())
        {
          motor.enableInertion();
          motor.setInertionTimeCoeff(1);
          speedMeasureTimer->restart();
          motor.setDuty(0);
          if (motor.getDuty() == 0)
          {
            testCase = INITIALIZATION_TESTING;
            speedMeasureTimer->set(15000);
            speedMeasureTimer->restart();
          }
        }
        break;
      case INITIALIZATION_TESTING:
        motor.setDuty(100);
        if (isAxisLocked())
        {
          testCase = WAIT_TO_STOP;
          motor.setDuty(0);
          speedMeasureTimer->set(20);
        }
        if (speedMeasureTimer->expired())
        {

          testCase = WAIT_TO_STOP;

          //testCase = IDLE_TEST;
        }
        break;
      case WAIT_TO_STOP:
        if (motor.getDuty() == 0)
        {
          expectedAngle = 40000; //20 degree
          testCase = IDLE_TEST;
        }
        break;
      case IDLE_TEST:
        {
        const int32_t staticError = labs(getStaticError());
        if (staticError > MAX_STATIC_ERROR_IDLE)
        {
          testCase = ROTATION_TEST;
        }
      }
        break;
      case ROTATION_TEST:
        {
        const int32_t staticError = getStaticError();
        const bool axisHoldPredicate = (labs(staticError) <= MAX_STATIC_ERROR_ROTATION);
        if (axisHoldPredicate)
        {
          motor.setDuty(0);
          brake.activate();
          testCase = IDLE_TEST;
        }
        else
        {
          brake.deactivate();
          const float staticErrorNorm = static_cast<float>(staticError) * REGULATOR_NORM_COEFF;
          const float regValue = regulator.getOut(staticErrorNorm);
          motor.setDuty(regValue * MAX_DUTY_VALUE);
        }
      }
        break;
      case STOP_TESTING:
        motor.setDuty(0);
        break;
      default:
        break;
    }
  }
}
