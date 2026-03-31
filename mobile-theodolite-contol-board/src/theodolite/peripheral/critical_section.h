/*
 * critical_section.h
 *
 *  Created on: 14 мая 2020 г.
 *      Author: maxim
 *  (c) 2020 NIC SPbETU
 */

#ifndef CRITICAL_SECTION_H_
#define CRITICAL_SECTION_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "stdbool.h"

void criticalSectionBegin();
void criticalSectionEnd();
bool enterRegion(uint8_t process);
void leaveRegion(uint8_t process);

#ifdef __cplusplus
}
#endif

#endif /* CRITICAL_SECTION_H_ */
