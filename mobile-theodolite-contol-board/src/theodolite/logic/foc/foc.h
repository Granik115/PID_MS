/*
 * foc.h
 *
 *  Created on: Aug 26, 2024
 *      Author: mvm
 */

#ifndef THEODOLITE_LOGIC_FOC_FOC_H_
#define THEODOLITE_LOGIC_FOC_FOC_H_

#include "BLDCMotor.h"
#include "foc/ssi_encoder/ssi_encoder.h"

bool focInit();
BLDCMotor& getAzimuthMotor();
BLDCMotor& getElevationMotor();
SSIEncoder* getElevationEncoder();
SSIEncoder* getAzimuthEncoder();

#endif /* THEODOLITE_LOGIC_FOC_FOC_H_ */
