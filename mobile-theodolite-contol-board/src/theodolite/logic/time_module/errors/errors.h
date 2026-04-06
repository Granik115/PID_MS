/*
 * errors.h
 *
 *  Created on: 8 июня 2016 г.
 *      Author: mmalenko
 *  (c) 2015 NIC SPbETU
 */

#include "bft.pb.h"

uint16_t getErrorCount(void);
timesync_messages_ErrorType getError(void);
void sendError(timesync_messages_ErrorType error);
