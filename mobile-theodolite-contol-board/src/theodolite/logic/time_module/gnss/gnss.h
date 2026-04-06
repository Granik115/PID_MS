/*
 * gnss.h
 *
 *  Created on: 7 апр. 2016 г.
 *      Author: mmalenko
 * (c) 2015 NIC SPbETU
 */

#ifndef SOURCE_LOGIC_GNSS_GNSS_H_
#define SOURCE_LOGIC_GNSS_GNSS_H_

#include "mnp_binary.h"
#include "firmware/ring_buffer/ring_buffer.h"
#include "firmware/third-party/nmeap/nmeap.h"
#include "firmware/link_level/uart_link.h"
#include "time.h"
#include "hardware.h"

typedef enum
{
  GNSS_INIT,
  GNSS_SEND_REQUEST,
  GNSS_WAIT_REPLY,
  GNSS_RUNNING,
  GNSS_RESET,
  GNSS_START,
  GNSS_ERROR
} GnssTaskState;

#define RMC_MSG_MIN_SIZE     80

const uint32_t GNSS_INIT_ATTEMPT_MAX = 10;

#define RMC_UNRELIABLE_FLAG   'V'
#define RMC_RELIABLE_FLAG     'A'

#include "stdint.h"

#define  GNSS_TASK_PRIORITY 1
#define MAX_NMEA_CNT 128
#define ASCII_SHIFT 48
#define BINARY_PORT_BAUDRATE 115200
#define NMEA_PORT_BAUDRATE 230400

typedef struct
{
  uint8_t *data;
  uint8_t size;
} GgaMessageStruct;

extern GgaMessageStruct ggaMessage;

#define ggaBuffAccesTimeOut 300

typedef enum
{
  GN,
  GP,
  GL
} GnssSource;

typedef enum
{
  NO_ERRORS,
  SYSTEM_ID_ERROR,
  MESSAGE_ID_ERROR,
  CS_ERROR,
  NO_SIGNAL,
  MISC_ERR
} GnssError;

typedef struct
{
  GnssSource source;
  GnssError error;
  //PulseTime time;
  uint16_t observationQuality;
  bool gnssState;
} GnssTime;

nmeap_rmc_t* getGnssRmcStruct(void);
nmeap_gga_t* getGnssGgaStruct(void);
void gnssTask(void);
time_t getGnssTimeSec(void);

#endif /* SOURCE_LOGIC_GNSS_GNSS_H_ */
