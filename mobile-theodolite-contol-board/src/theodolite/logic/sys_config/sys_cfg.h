/*
 * sys_cfg.h
 *
 *      Author: mmalenko
 * (c) 2017 NIC SPbETU
 */

#ifndef PERIPHERAL_SYS_CFG_H_
#define PERIPHERAL_SYS_CFG_H_

#include "stdint.h"

const float AUTOMATIC_MOVE_INERTION_TIME = 0.1; //0.3; //c
const float STOPPING_MOVE_INERTION_TIME = 0.2;
const float DEFAULT_INTEGRATOR_TIME = AUTOMATIC_MOVE_INERTION_TIME; //c

const uint32_t MAX_AZIMUTH_EXPLICIT_ANGLE = 359;
const uint32_t MIN_AZIMUTH_EXPLICIT_ANGLE = 0;
const uint32_t MAX_ELEVATION_EXPLICIT_ANGLE = 230;
const uint32_t MIN_ELEVATION_EXPLICIT_ANGLE = 0;

void systemConfiguration();
void periodicCallsConfig();

#endif /* PERIPHERAL_SYS_CFG_H_ */
