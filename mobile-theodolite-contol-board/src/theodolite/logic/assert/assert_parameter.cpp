/*
 * assert.c
 *
 *  Created on: 12 апр. 2016 г.
 *      Author: mmalenko
 *
 *    (c) 2015 NIC SPbETU
 */

#include <theodolite/logic/assert/assert_parameter.h>
#include "theodolite/logic/pc_protocol/third-party/theodolite_messages.pb.h"

void assert_failed(uint_least8_t* file, uint32_t line)
{
  //printf("\nAssert failed! line:%lu\nfile:%s\n", line, file);
}
