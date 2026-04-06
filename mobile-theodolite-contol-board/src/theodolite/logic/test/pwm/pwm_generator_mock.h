/*
 * pwm_generator_mock.h
 *
 *  Created on: 9 авг. 2017 г.
 *      Author: mmalenko
 *(c) 2017 NIC SPbETU
 */

#ifndef PWM_GENERATOR_MOCK_H_
#define PWM_GENERATOR_MOCK_H_

#include <gmock/gmock.h>
#include <theodolite/logic/pwm/pwm_interface.h>

class PwmMock:
  public PwmInterface
{
public:
  MOCK_METHOD0(enableInertion,void());
  MOCK_METHOD0(disableInertion,void());
  MOCK_METHOD1(setInertionTimeCoeff,void(float coeff));
  MOCK_METHOD1(setDuty,void(float percent));
  MOCK_METHOD0(getDuty,float());
  MOCK_METHOD0(getTimerDuty,int16_t());
  MOCK_METHOD0(update,void());
  MOCK_METHOD0(setUpdateRequest,void());
};

#endif /* PWM_GENERATOR_MOCK_H_ */
