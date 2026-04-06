/*
 * assert.c
 *
 *  Created on: 12 апр. 2016 г.
 *      Author: mmalenko
 *
 *    (c) 2015 NIC SPbETU
 */

#include "time_module/assert/assert_parameter.h"

void assert_failed(uint8_t *file, uint32_t line)
{
#if __have_long32
  printf("\nAssert failed! line:%lu\nfile:%s\n", line, file);
#else
  printf("\nAssert failed! line:%u\nfile:%s\n", line, file);
#endif
}
