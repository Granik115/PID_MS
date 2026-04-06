/*
 * v_sev_logic.c
 *
 *  Created on: 12 окт. 2015 г.
 *      Author: mmalenko
 *
 * (c) 2015 NIC SPbETU
 */

#include "time_module/digital_sev/v_sev_logic.h"
#include "time_module/errors/errors.h"

#define VSEV_PACK_MAX_LEN  15   //Длина пакета Вешняк
#define VSEV_CRC_CALK_LEN  10   //Длина участка для расчета CRC
#define VSEV_CRC_SHIFT     11   //Смещение CRC относительно начала пакета

static RingBuffer vSevGnssUartBuffer;
static vSevTimeCodeState state = WAIT_FLAG;

void resetVSevParsing()
{
  state = WAIT_FLAG;
}

static vSevTimeCode timeCode;
static PulseTime vSevTime;
static uint8_t vSevTimeCodeBuff[TIMECODE_LEN] =
{V_SEV_ID};

RingBuffer * getVSevRingBuffer()
{
  return &vSevGnssUartBuffer;
}

typedef enum
{
  DETECT_START,
  NEXT_BYTE,
  PCK_RCVD
} VSevFsmMsg;

VSevFsmMsg vSevFSM(uint8_t byte)
{
  static uint16_t crc = 0;
  static uint8_t timeCodeCnt = 1;
  switch (state)
  {
    case WAIT_FLAG:
      if (byte == V_SEV_FLAG)
      {
        state = WAIT_ID;
        return DETECT_START;
      }
      break;
    case WAIT_ID:
      if (byte == V_SEV_ID)
      {
        state = WAIT_DATA;
      }
      else
      {
        state = WAIT_FLAG;
      }
      break;
    case WAIT_DATA:
      vSevTimeCodeBuff[timeCodeCnt++] = byte;
      if ((timeCodeCnt == 10))
      {
        timeCodeCnt = 1;
        state = WAIT_CRC;
      }
      return NEXT_BYTE;
    case WAIT_CRC:
      {
    }
      static uint8_t crcCnt = 0;
      if (crcCnt == 0)
      {
        crc = 0;
        crc |= ((uint16_t)(byte)) << 8;
        crcCnt = 1;
      }
      else
      {
        crc |= (uint16_t)(byte);
        state = WAIT_SECOND_FLAG;
        crcCnt = 0;
      }
      return NEXT_BYTE;
    case WAIT_SECOND_FLAG:
      if (byte == V_SEV_FLAG)
      {
        state = WAIT_END;
        return NEXT_BYTE;
      }
      else
      {
        state = WAIT_FLAG;
        return NEXT_BYTE;
      }
    case WAIT_END:
      if (byte == V_SEV_END)
      {
        state = WAIT_FLAG;
        if (crc == crc16(vSevTimeCodeBuff, 10))
        {
          timeCode = vSevParseTimeCode(vSevTimeCodeBuff);
          timeCode.error = NO_SEV_ERRORS;
          vSevTime.time = timeCode.time;
          if (timeCode.status & TIME_SCALE_UNRELIABLE_FLAG)
          {
            sendError(timesync_messages_ErrorType_DIGITAL_SEV_TIMECODE_UNRELIABLE_ERROR);
            timeCode.error = TIME_UNRELIABLE_ERROR;
          }
          return PCK_RCVD;
        }
        else
        {
          timeCode.error = CRC_ERROR;
          sendError(timesync_messages_ErrorType_DIGITAL_SEV_TIMECODE_CRC_ERROR);
        }
      }
      else
      {
        state = WAIT_FLAG;
        return NEXT_BYTE;
      }
      break;
    default:
      break;
  }
  return NEXT_BYTE;
}

uint8_t inTaskProcess()
{
  static VSevFsmMsg msg = NEXT_BYTE;
  if (msg == DETECT_START)
  {
    if (getRingBufferCount(&vSevGnssUartBuffer) > (VSEV_CRC_SHIFT))
    {
      uint16_t crc_h = ((uint16_t)showSymbolFromRingBuffer(VSEV_CRC_SHIFT - 1,
                                                           &vSevGnssUartBuffer));
      uint16_t crc_l = ((uint16_t)showSymbolFromRingBuffer(VSEV_CRC_SHIFT,
                                                           &vSevGnssUartBuffer));
      uint16_t crc16 = (crc_h << 8) | crc_l;
      msg = NEXT_BYTE;
      uint16_t currentCRC = crc16InRingBufferCalculate(&vSevGnssUartBuffer, VSEV_CRC_CALK_LEN, 0);
      if (crc16
      != currentCRC)
      {
        resetVSevParsing();
        getSymbolFromRingBuffer(&vSevGnssUartBuffer);
      }
    }
  }
  else if (getRingBufferCount(&vSevGnssUartBuffer))
  {
    msg = vSevFSM(getSymbolFromRingBuffer(&vSevGnssUartBuffer));
    if (msg == PCK_RCVD)
    {
      return 1;
    }
  }
  return 0;
}

/*Разбор кода времени
 * Принимает строку, содержащую код времени сев.
 * */
vSevTimeCode vSevParseTimeCode(uint8_t *timeCodeStr)
{
  uint8_t cursor = 1;
  vSevTimeCode sevTimeCode;
  sevTimeCode.status = *(timeCodeStr + cursor++);
  sevTimeCode.time.tm_hour = *(timeCodeStr + cursor++);
  sevTimeCode.time.tm_min = *(timeCodeStr + cursor++);
  sevTimeCode.time.tm_sec = *(timeCodeStr + cursor++);
  sevTimeCode.time.tm_mday = *(timeCodeStr + cursor++);
  sevTimeCode.time.tm_mon = *(timeCodeStr + cursor++) - 1;
  sevTimeCode.time.tm_year = 100 + *(timeCodeStr + cursor++);
  sevTimeCode.time.tm_isdst = -1;
  sevTimeCode.utcCorrection = ((uint16_t)*(timeCodeStr + (cursor)) << 8) | (uint16_t)*(timeCodeStr + (cursor + 1));
  return sevTimeCode;
}

PulseTime *getVSevTimeStruct()
{
  return &vSevTime;
}

time_t getVSevTimeSec()
{
  struct tm timeStruct;
  time_t digitalSevTime;
  timeStruct = vSevTime.time;
  digitalSevTime = mktime(&timeStruct);
  return digitalSevTime;
}

vSevTimeCode *getVSevTimeCode()
{
  return &timeCode;
}

