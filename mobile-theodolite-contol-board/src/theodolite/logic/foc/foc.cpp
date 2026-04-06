/*
 * foc.cpp
 *
 *  Created on: Aug 26, 2024
 *      Author: mvm
 */

#include "cmsis_os2.h"
#include "foclib_driver/MKTIBLDCDriver.h"
#include "BLDCMotor.h"
#include "foclib_current_sense/current_sensor_adc.h"
#include "foclib_current_sense/MKTILowsideCurrentSense.h"
#include "cpu_board/config/config.h"
#include "ssi_encoder/ssi_encoder.h"
#include "communication/Commander.h"
#include "USBSerial.h"

namespace
{
  const float VELOCITY_P = 20.4;
  const float VELOCITY_I = 470;
  const float ANGLE_P = 35;
  const float MOTOR_VOLTAGE_LIMIT = 20;
  const float VELOCITY_LIMIT = 0.7;
  const float MOTOR_CURRENT_LIMIT = 10;
}

//JCM115x25S
const int JCM115x25S_N_POLE_PAIR = 15;
const float JCM115x25S_R = 1.35 / 2; // Ohm one phase
const float JCM115x25S_KV = 10.8;
const float JCM115x25S_L = 2.6 / (1000 * 2); //One phase in H

const uint32_t ABS_ENCODER_RES = 21;
const int MOTOR_SUPPLY_V = 48;
//Encoder encoder = Encoder(2, 3, 8192);
static void focTask(void *argument);
static osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes =
{
  .name = "foc_task",
  .stack_size = 15000,
  .priority = (osPriority_t)osPriorityHigh,
};

SSIEncoder *p_azimuthEncoder;
SSIEncoder *p_elevationEncoder;

static BLDCMotor azimuthMotor = BLDCMotor(JCM115x25S_N_POLE_PAIR, JCM115x25S_R, JCM115x25S_KV, JCM115x25S_L);
static BLDCMotor elevationMotor = BLDCMotor(JCM115x25S_N_POLE_PAIR, JCM115x25S_R, JCM115x25S_KV, JCM115x25S_L);
static SSIEncoder azimuthEncoder = SSIEncoder(ABS_ENCODER_RES, true, AZIMUTH_SSI_DATA_BB, AZIMUTH_SSI_CLK_BB);
static SSIEncoder elevationEncoder = SSIEncoder(ABS_ENCODER_RES, true, ELEVATION_SSI_DATA_BB, ELEVATION_SSI_CLK_BB);

bool focInit()
{
  SimpleFOCDebug::enable(&SerialUSB);
  defaultTaskHandle = osThreadNew(focTask, NULL, &defaultTask_attributes);
  return true;
}

Commander commander = Commander(SerialUSB, '\n', false);
static BLDCMotor *p_azMotor = NULL;

void doSomething(char *cmd)
{
  SerialUSB.printf("doSomething %s\n", cmd);
}

void setUsbToUart(char *cmd)
{

}

void doMotor(char *cmd)
{
  commander.motor(p_azMotor, cmd);
}

static void initMotor(BLDCMotor &motor)
{
  motor.controller = MotionControlType::angle;
  motor.torque_controller = TorqueControlType::voltage;
  // controller configuration based on the control type
  // velocity PI controller parameters
  // default P=0.5 I = 10
  motor.PID_velocity.P = VELOCITY_P;
  motor.PID_velocity.I = VELOCITY_I;
  // jerk control using voltage voltage ramp
  // default value is 300 volts per sec  ~ 0.3V per millisecond
  motor.PID_velocity.output_ramp = 1000;

  //default voltage_power_supply
  motor.voltage_limit = MOTOR_VOLTAGE_LIMIT;
  motor.current_limit = MOTOR_CURRENT_LIMIT;

  // velocity low pass filtering
  // default 5ms - try different values to see what is the best.
  // the lower the less filtered
  motor.LPF_velocity.Tf = 0.01;

  // angle P controller
  // default P=20
  motor.P_angle.P = ANGLE_P;
  //  maximal velocity of the position control
  // default 20
  motor.velocity_limit = VELOCITY_LIMIT;
  motor.zero_electric_angle = 0;
  //motor.zero_electric_angle = 0;
  motor.sensor_direction = Direction::CW;
  motor.useMonitoring(SerialUSB);
  motor.init();
  motor.initFOC();
}

static void focTask(void *argument)
{
  p_azimuthEncoder = &azimuthEncoder;
  AzimuthMotorPwmTimer azTimer;
  MKTIBLDCDriver azDriver = MKTIBLDCDriver(azTimer);
  azDriver.voltage_power_supply = MOTOR_SUPPLY_V;
  azDriver.voltage_limit = MOTOR_VOLTAGE_LIMIT;
  azDriver.init();

  AzimuthMotorCurrentAdc azAdc;
  MKTILowsideCurrentSense azimuthCurrentSense = MKTILowsideCurrentSense(0.01, 100, azAdc);
  azimuthCurrentSense.skip_align = true;
  azimuthCurrentSense.linkDriver(&azDriver);
  azimuthCurrentSense.init();

  azimuthMotor.linkDriver(&azDriver);
  azimuthMotor.linkCurrentSense(&azimuthCurrentSense);
  azimuthMotor.linkSensor(p_azimuthEncoder);
  initMotor(azimuthMotor);
  p_azMotor = &azimuthMotor;

  commander.add('A', doMotor, "azimuth motor");
  // commander.add('I', onCommand, label);
  uint32_t micros_prev = 0;
  volatile uint32_t delta_micros = 0;
  UNUSED(delta_micros);
  for (;;)
  {
    LL_GPIO_TogglePin(TRIGGER_GPIO_Port, TRIGGER_Pin);
    uint32_t micros_current = MKS_TIM->CNT;      //HAL_GetTick();//micros();
    delta_micros = micros_current - micros_prev;
    micros_prev = micros_current;
    commander.run();
    //azMotor.loopFOC();
    azimuthMotor.monitor();
    LL_GPIO_TogglePin(TRIGGER_GPIO_Port, TRIGGER_Pin);
    //azMotor.move(NOT_SET);
    osDelay(1);
  }
}

SSIEncoder* getAzimuthEncoder()
{
  return &azimuthEncoder;
}

SSIEncoder* getElevationEncoder()
{
  return &elevationEncoder;
}

BLDCMotor& getAzimuthMotor()
{
  return azimuthMotor;
}

BLDCMotor& getElevationMotor()
{
  return elevationMotor;
}

extern "C"
{
void TIM8_BRK_TIM12_IRQHandler(void)
{
  if (LL_TIM_IsActiveFlag_BRK(TIM8))
    LL_TIM_ClearFlag_BRK(TIM8);

  if (LL_TIM_IsActiveFlag_UPDATE(TIM12))
  {
    LL_TIM_ClearFlag_UPDATE(TIM12);
    bool isAzUpdate = false;
    bool isElUpdate = false;
    if (p_azimuthEncoder)
      isAzUpdate = p_azimuthEncoder->isrHandler();
    if (p_elevationEncoder)
      isElUpdate = p_elevationEncoder->isrHandler();
    if (!isAzUpdate && !isElUpdate)
    {
      LL_TIM_DisableIT_UPDATE(TIM12);
    }
  }
}

void TIM7_IRQHandler(void)
{
  if (LL_TIM_IsActiveFlag_UPDATE(TIM7))
  {
    LL_TIM_ClearFlag_UPDATE(TIM7);
    if (p_azMotor)
    {
      p_azMotor->loopFOC();
      p_azMotor->move(NOT_SET);
    }
  }
}

}

