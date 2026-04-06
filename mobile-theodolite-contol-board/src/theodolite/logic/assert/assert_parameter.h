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
#include "firmware/errors/errors.h"
#include "theodolite/logic/pc_protocol/third-party/theodolite_messages.pb.h"

//#define assert_parameter(expr) ((expr) ? (void)0 : assert_failed((uint8_t *)__FILE__, __LINE__))
/* Exported functions ------------------------------------------------------- */
//void assert_failed(uint_least8_t* file, uint32_t line);
//#endif

ErrorsStorage<theodolite_messages_Error> &getErrorStorage();

#endif /* SOURCE_LOGIC_TST_ASSERT_H_ */
