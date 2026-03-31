/*
 * hardware.h
 *
 *      Author: mmalenko
 * (c) 2024 NIC SPbETU
 */

#ifndef INCLUDE_HARDWARE_H_
#define INCLUDE_HARDWARE_H_

#include "stm32f4xx_ll_tim.h"
#include "hal.h"
#include "theodolite/logic/sys_config/sys_cfg.h"
#include "theodolite/logic/periph_bridge/pwm.h"
#include "theodolite/logic/periph_bridge/gpio/gpio.h"
#include "theodolite/logic/periph_bridge/uart.h"
#include "theodolite/peripheral/common/gpio/gpio.h"

#define __interrupt
#define interrupt

const uint32_t LSPCLK_FREQ = 75000000;
const int16_t PWM_TIMER_PERIOD_MAX = 7500; //3750; //40 kHZ
const bool HIGH_LEVEL = true;
const bool LOW_LEVEL = false;
const bool BRAKE_ACTIVE_STATE = HIGH_LEVEL;
const bool BRAKE_INACTIVE_STATE = !BRAKE_ACTIVE_STATE;


#define FREQ_DEVIATION (500) //ppm
#define PERIOD_DEVIATION (150) //в единицах счетчика таймера
#define TIM_FREQ_CONST (84000000)
#define FPGA_NEGEDGE_CNT_VALUE     ((uint32_t)5000)
#define FPGA_SYNC_MIN_DIFFERENCE   ((uint32_t)10)
#define INTERRUPT_EXECUTE_TIME     ((uint32_t)0)//данная величина зависит от системной частоты
#define SYNC_TIMERS_PERIOD  ((uint32_t)(TIM_FREQ_CONST * 2))



#endif /* INCLUDE_HARDWARE_H_ */
