/*
 * iso_time.cpp
 *
 *  Created on: 26 янв. 2016 г.
 *      Author: mmalenko
 * (c) 2016 NIC SPbETU
 */

#include "iso_time.h"
#include "string.h"
#include "stdio.h"

const uint8_t ASCII_SHIFT = 48;
const uint8_t ISO_STRING_MAX_LEN = 23;

/*формат ISO 8601: YYYYMMDDThhmmss.ffffff
 * */
bool decodeIsoTime(PulseTime *time, const char *str)
{
  if (strlen(str) > ISO_STRING_MAX_LEN)
  {
    return false;
  }

  uint32_t year = (*str - ASCII_SHIFT) * 1000 + ((*(str + 1) - ASCII_SHIFT) * 100)
  + ((*(str + 2) - ASCII_SHIFT) * 10)
  + ((*(str + 3) - ASCII_SHIFT));
  if (year >= 1900)
  {
    time->time.tm_year = year - 1900;
  }
  else
  {
    time->time.tm_year = 0;
  }
  str += 4;
  uint32_t mon = ((*str - ASCII_SHIFT) * 10)
  + ((*(str + 1) - ASCII_SHIFT));
  if (mon > 0)
  {
    time->time.tm_mon = mon - 1;
  }
  else
  {
    time->time.tm_mon = 0;
  }
  str += 2;
  time->time.tm_mday = ((*str - ASCII_SHIFT) * 10)
  + ((*(str + 1) - ASCII_SHIFT));
  str += 3; //пропускаем символ T
  time->time.tm_hour = ((*str - ASCII_SHIFT) * 10)
  + ((*(str + 1) - ASCII_SHIFT));
  str += 2;
  time->time.tm_min = ((*str - ASCII_SHIFT) * 10)
  + ((*(str + 1) - ASCII_SHIFT));
  str += 2;
  time->time.tm_sec = ((*str - ASCII_SHIFT) * 10)
  + ((*(str + 1) - ASCII_SHIFT));
  str += 3; //пропускаем .
  time->mkSec = (*str - ASCII_SHIFT) * 100000 + ((*(str + 1) - ASCII_SHIFT) * 10000)
  + ((*(str + 2) - ASCII_SHIFT) * 1000)
  + ((*(str + 3) - ASCII_SHIFT) * 100) + ((*(str + 4) - ASCII_SHIFT) * 10) + ((*(str + 5) - ASCII_SHIFT));
  time->time.tm_isdst = -1;
  return true;
}

bool encodeIsoTime(const PulseTime &time, char *str)
{
#if __have_long32
  if (snprintf(str, ISO_STRING_MAX_LEN, "%04d%02d%02dT%02d%02d%02d.%06lu", (time.time.tm_year + 1900),
#else
  if (snprintf(str, ISO_STRING_MAX_LEN, "%04d%02d%02dT%02d%02d%02d.%06u", (time.time.tm_year + 1900),
#endif
               (time.time.tm_mon + 1),
               time.time.tm_mday,
               time.time.tm_hour,
               time.time.tm_min,
               time.time.tm_sec,
               time.mkSec) != -1)
  {
    return true;
  }
  return false;
}

