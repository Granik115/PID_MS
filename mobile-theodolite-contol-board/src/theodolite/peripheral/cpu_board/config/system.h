/*
 * system.h
 *
 *  Created on: 6 мар. 2021 г.
 *      Author: maxim
 */

#ifndef SRC_SYSTEM_SYSTEM_H_
#define SRC_SYSTEM_SYSTEM_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "cmsis_os.h"
#include "stddef.h"
#include "firmware/ring_buffer/ring_buffer.h"

void init_debug();
void rebootDevice();
void reboot_message();
void resetWatchDog();
RingBuffer* getDebugTxRingBuffer();
RingBuffer* getDebugRxRingBuffer();
void* pvPortCalloc(size_t number, size_t xWantedSize);
void* pvPortRealloc(void *ptr, size_t s);
void* malloc(size_t n);
void free(void *pv);
void* calloc(size_t number, size_t xWantedSize);
void* realloc(void *ptr, size_t s);
int rand_h();
int platform_printf(const char*__restrict, ...);
int mqtt_printf(const char *__restrict message, ...);
int _write(int file, char *ptr, int len);
uint32_t get_uptime_s();
uint8_t get_proc_load();
uint8_t get_mem_load();
uint8_t get_max_mem_load();

#ifdef __cplusplus
}
#endif

#endif /* SRC_SYSTEM_SYSTEM_H_ */
