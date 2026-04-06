/*
 * iso_time.h
 *
 *  Created on: 26 янв. 2016 г.
 *      Author: mmalenko
 *      
 * (c) 2016 NIC SPbETU
 */

#ifndef SOURCE_PROTOCOL_DEPEND_ISO_TIME_H_
#define SOURCE_PROTOCOL_DEPEND_ISO_TIME_H_

#include "porttime.h"
#include "stdbool.h"

bool encodeIsoTime(const PulseTime &time, char *str);
bool decodeIsoTime(PulseTime *time, const char *str);

#endif /* SOURCE_PROTOCOL_DEPEND_ISO_TIME_H_ */
