/*
 * version.h
 *
 *  Created on: 21 июн. 2019 г.
 *      Author: mmalenko
 *  (c) 2019 NIC SPbETU
 */

#ifndef SRC_THEODOLITE_LOGIC_MAIN_VERSION_H_
#define SRC_THEODOLITE_LOGIC_MAIN_VERSION_H_

volatile const char app_version[] __attribute__((section(".application_version")))
= "arm cpu mobile logic v1.0.0";

#endif /* SRC_THEODOLITE_LOGIC_MAIN_VERSION_H_ */
