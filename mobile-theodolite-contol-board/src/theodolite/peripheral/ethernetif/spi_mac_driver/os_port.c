/*
 * os_port.c
 *
 *  Created on: 10 июл. 2024 г.
 *      Author: mvm
 */
#include "os_port.h"
#include "os_rtos.h"

 bool_t osSetEventFromIsr(OsEvent *event)
 {
    //Set the specified event to the signaled state

    //The return value is not relevant
    return FALSE;
 }


