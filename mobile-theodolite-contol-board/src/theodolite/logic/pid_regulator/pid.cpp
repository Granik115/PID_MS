/*
 * pid.cpp
 *
 *  Created on: 18 янв. 2017 г.
 *      Author: mmalenko
 * (c) 2017 NIC SPbETU
 */

#include "pid.h"

PidRegulator::PidRegulator(float kp = 0, float ki = 0, float kd = 0, float kc = 0, float outMax = 0, float outMin = 0)
{
  this->kp = kp;
  this->ki = ki;
  this->kd = kd;
  this->kc = kc;
  this->outMax = outMax;
  this->outMin = outMin;
  ui = 0;
  up1 = 0;
  satErr = 0;
}

PidRegulator::PidRegulator()
{

}

void PidRegulator::setOutMax(float outMax)
{
  this->outMax = outMax;
}
void PidRegulator::setOutMin(float outMin)
{
  this->outMin = outMin;
}
void PidRegulator::setKi(float ki)
{
  this->ki = ki;
}
void PidRegulator::setKp(float kp)
{
  this->kp = kp;
}
void PidRegulator::setKc(float kc)
{
  this->kc = kc;
}
void PidRegulator::setKd(float kd)
{
  this->kd = kd;
}

float PidRegulator::getKd()
{
  return kd;
}

float PidRegulator::getKi()
{
  return ki;
}
float PidRegulator::getKp()
{
  return kp;
}
float PidRegulator::getKc()
{
  return kc;
}

void PidRegulator::reset()
{
  satErr = 0;
  ui = 0;
}

float PidRegulator::getOut(float err)
{
  const float up = kp * err;
  ui = ui + ki * err + kc * satErr;

  if (ki == 0)
  {
    ui = 0;
  }
  float ud = kd * (up - up1);
  const float outPreSat = up + ui + ud;

  float out;
  if (outPreSat > outMax)
  {
    out = outMax;
  }
  else if (outPreSat < outMin)
  {
    out = outMin;
  }
  else
  {
    out = outPreSat;
  }

  // Compute the saturate difference
  satErr = out - outPreSat;

  // Update the previous proportional output
  up1 = up;
  return out;
}

