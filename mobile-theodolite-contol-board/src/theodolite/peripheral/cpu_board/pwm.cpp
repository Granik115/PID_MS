/*
 * pwm.cpp
 *
 *      Author: mmalenko
 * (c) 2017 NIC SPbETU
 */

#include <theodolite/logic/axis_mover/axis_mover_interface.h>
#include <theodolite/logic/local_clock/local_clock.h>
#include "theodolite/peripheral/hardware.h"
#include "theodolite/logic/software_uart/sw_uart.h"

interrupt void softUartTimerIsr(void);
extern void periodicIsrCall(void);

void isrSoftUartCallBack() //Call in timer interrupt
{

}

void disableAllInterrupt()
{

}

void enableSoftUartTimerInterrupt()
{

}

void disableSoftUartTimerInterrupt()
{

}

void restoreAllInterrupt()
{

}

void clearSoftUartTimer()
{

}

inline uint16_t getSoftUartTimerValue()
{
  return 0;
}

void softUartTimerRoutine()
{
  //disableAllInterrupt();
  //enableSoftUartTimerInterrupt();
  //disableSoftUartTimerInterrupt();
}

inline void isrCameraConnectorCallback()
{

}

extern inline void isrCameraConnectorCallback();

__interrupt void localMsecTimingIsr(void)
{
  isrCameraConnectorCallback();
  periodicIsrCall();
}

__interrupt void softUartTimerIsr(void)
{
  isrSoftUartCallBack();
}

const uint32_t CAMERA_UART_BOUDRATE = 115200;
const uint32_t SYS_CLK = 150000000;
const uint16_t SOFT_UART_TIM_PERIOD = SYS_CLK / CAMERA_UART_BOUDRATE;

void periodicCallsConfig()
{
  pwmConfig();
}

void pwmConfig()
{
  //PieVectTable.T2UFINT = &softUartTimerIsr;  //
  //PieVectTable.T4UFINT = &localMsecTimingIsr;
}

void AzimuthMotor::setPeripheralIncrementDirection()
{

}
void AzimuthMotor::setPeripheralDecrementDirection()
{

}

void ElevationMotor::setPeripheralIncrementDirection()
{

}

void ElevationMotor::setPeripheralDecrementDirection()
{

}
