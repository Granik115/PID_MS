/*
 * errors.c
 *
 *  Created on: 8 июня 2016 г.
 *      Author: mmalenko
 * (c) 2015 NIC SPbETU
 */

#include "errors.h"
#include "firmware/ring_buffer/ring_buffer.h"

static RingBuffer errorsBuffer;

const char errorsStringArray[9][40] =
{"SEV_PPS_PERIOD_ERROR",
  "SEV_PPS_MISS_ERROR",
  "SEV_PPM_MISS_ERROR",
  "GNSS_PPS_MISS_ERROR",
  "GNSS_PPS_PERIOD_ERROR",
  "GNSS_MESSAGE_MISS_ERROR",
  "GNSS_SOLUTION_UNRELIABLE_ERROR",
  "DIGITAL_SEV_TIMECODE_UNRELIABLE_ERROR",
  "DIGITAL_SEV_TIMECODE_MISS_ERROR"};

void sendError(timesync_messages_ErrorType error)
{
  putSymbolToRingBuffer(error, &errorsBuffer);
}

uint16_t getErrorCount()
{
  return getRingBufferCount(&errorsBuffer);
}

timesync_messages_ErrorType getError()
{
  return static_cast<timesync_messages_ErrorType>(getSymbolFromRingBuffer(&errorsBuffer));
}
