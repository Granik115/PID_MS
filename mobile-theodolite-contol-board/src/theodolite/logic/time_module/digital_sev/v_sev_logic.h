/*
 * v_sev_logic.h
 *
 *  Created on: 12 окт. 2015 г.
 *      Author: mmalenko
 * (c) 2015 NIC SPbETU
 */

#include "firmware/link_level/crc16.h"
#include "firmware/ring_buffer/ring_buffer.h"
#include "time_module/time/porttime.h"

#ifndef SOURCE_DRIVER_V_SEV_LOGIC_V_SEV_LOGIC_H_
#define SOURCE_DRIVER_V_SEV_LOGIC_V_SEV_LOGIC_H_

#define V_SEV_FLAG  0xAA
#define V_SEV_ID    0x18
#define V_SEV_END   0xA5

#define TIME_SCALE_RELIABLE_FLAG     0x0
#define TIME_SCALE_UNRELIABLE_FLAG   0x1

#define AUTOMATIC_MODE_FLAG          0x0
#define AUTONOMUS_MODE_FLAG          0x2

#define TIME_CODE_TASK_PRIORITY       1
#define TIMECODE_LEN                 10
#define V_SEV_PACKAGE_LEN            15
#define SERIAL_BUFFER_SIZE           40

typedef enum
{
  CRC_ERROR,
  TIMEOUT_ERROR,
  NO_PACKET_ERROR,
  TIME_UNRELIABLE_ERROR,
  NO_SEV_ERRORS,
} VSevError;

typedef enum
{
  WAIT_FLAG,
  WAIT_ID,
  WAIT_DATA,
  WAIT_CRC,
  WAIT_SECOND_FLAG,
  WAIT_END
} vSevTimeCodeState;

typedef struct
{
  uint8_t status;
  struct tm time;
  uint16_t utcCorrection;
  VSevError error;
} vSevTimeCode;

vSevTimeCode vSevParseTimeCode(uint8_t *timeCodeStr);
PulseTime *getVSevTimeStruct(void);
time_t getVSevTimeSec(void);
vSevTimeCode *getVSevTimeCode(void);
RingBuffer * getVSevRingBuffer(void);
uint8_t inTaskProcess(void);
void resetVSevParsing(void);

#endif /* SOURCE_DRIVER_V_SEV_LOGIC_V_SEV_LOGIC_H_ */
