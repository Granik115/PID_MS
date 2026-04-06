/*
 * pid_reg_mock.h
 *
 *  Created on: 19 мая 2017 г.
 *      Author: mmalenko
 * (c) 2017 NIC SPbETU       
 */

#ifndef PID_REG_MOCK_H_
#define PID_REG_MOCK_H_

#include <gmock/gmock.h>

class PidRegMock:
  public PidRegulatorInterface
{
public:
  MOCK_METHOD1(setKi,void(float ki));
  MOCK_METHOD1(setKp,void(float ki));
  MOCK_METHOD1(setKc,void(float ki));
  MOCK_METHOD1(setKd,void(float ki));
  MOCK_METHOD1(getOut,float(float err));
  MOCK_METHOD0(getKi,float(void));
  MOCK_METHOD0(getKp,float(void));
  MOCK_METHOD0(getKc,float(void));
  MOCK_METHOD0(getKd,float());
  MOCK_METHOD0(reset,void());
};

#endif /* PID_REG_MOCK_H_ */
