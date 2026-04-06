/*
 * assert.h
 *
 *  Created on: 12 апр. 2016 г.
 *      Author: mmalenko
 *  (c) 2015 NIC SPbETU
 */

#ifndef SOURCE_LOGIC_TST_ASSERT_H_
#define SOURCE_LOGIC_TST_ASSERT_H_

#include "stdint.h"
#include "stdio.h"

#define assert_parameter(expr) ((expr) ? (void)0 : assert_failed((uint8_t *)__FILE__, __LINE__))
/* Exported functions ------------------------------------------------------- */
void assert_failed(uint8_t* file, uint32_t line);

#endif /* SOURCE_LOGIC_TST_ASSERT_H_ */
