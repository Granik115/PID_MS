/*
 * pid_regulator_interface.h
 *
 *  Created on: 22 марта 2017 г.
 *      Author: mmalenko
 * (c) 2017 NIC SPbETU
 */

#ifndef LOGIC_PID_REGULATOR_PID_REGULATOR_INTERFACE_H_
#define LOGIC_PID_REGULATOR_PID_REGULATOR_INTERFACE_H_

class PidRegulatorInterface
{
public:
  virtual void setKi(float ki)=0;
  virtual void setKp(float kp)=0;
  virtual void setKc(float kc)=0;
  virtual void setKd(float kd)=0;
  virtual float getOut(float err)=0;
  virtual float getKi()=0;
  virtual float getKp()=0;
  virtual float getKc()=0;
  virtual float getKd()=0;
  virtual void reset()=0;
  virtual ~PidRegulatorInterface()
  {

  }
};

#endif /* LOGIC_PID_REGULATOR_PID_REGULATOR_INTERFACE_H_ */
