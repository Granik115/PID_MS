/*
 * sys_cfg.cpp
 *
 *      Author: mmalenko
 * (c) 2016 NIC SPbETU
 */

#include "main.h"
#include "gpio.h"
#include "dma.h"
#include "rng.h"
#include "tim.h"
#include "adc.h"
#include <theodolite/logic/sys_config/sys_cfg.h>
#include "theodolite/peripheral/hardware.h"
#include <theodolite/logic/external_controls/controls.h>
#include "system.h"

extern "C"
{
void SystemClock_Config(void);
void MX_FREERTOS_Init(void);
}

void systemConfiguration()
{
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_RNG_Init();
  MX_ADC3_Init();
  MX_TIM1_Init();
  MX_TIM8_Init();
  MX_TIM12_Init();
  syncPwmTimers();
  MX_TIM7_Init();
  uartConfig();
  init_debug();
  //MX_FREERTOS_Init();
}






