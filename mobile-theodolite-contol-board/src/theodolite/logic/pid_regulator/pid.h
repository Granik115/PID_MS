/*
 * pid.h
 *
 *  Created on: 18 янв. 2017 г.
 *      Author: mmalenko
 * (c) 2017 NIC SPbETU
 */

#ifndef LOGIC_PID_REGULATOR_PID_H_
#define LOGIC_PID_REGULATOR_PID_H_

#include "pid_regulator_interface.h"

class PidRegulator:
  public PidRegulatorInterface
{
  float kp;      // Parameter: Proportional gain
  float ui;      // Variable: Integral output
  float outMax;    // Parameter: Maximum output
  float outMin;    // Parameter: Minimum output
  float satErr;    // Variable: Saturated difference
  float ki;      // Parameter: Integral gain
  float kc;      // Parameter: Integral correction gain
  float kd;      // Parameter: Derivative gain
  float up1;     // History: Previous proportional output
public:
  PidRegulator();
  PidRegulator(float kp, float ki, float kd, float kc, float outMax, float outMin);
  void setOutMax(float outMax);
  void setOutMin(float outMin);
  void setKi(float ki);
  void setKp(float kp);
  void setKc(float kc);
  void setKd(float kd);
  float getKi();
  float getKp();
  float getKc();
  float getKd();
  float getOut(float err);
  void reset();
};

#endif /* LOGIC_PID_REGULATOR_PID_H_ */
