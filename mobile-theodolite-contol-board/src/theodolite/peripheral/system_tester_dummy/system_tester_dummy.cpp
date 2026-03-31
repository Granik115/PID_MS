///*
// * memory_dummy.cpp
// *
// *  Created on: 16 июл. 2018 г.
// *      Author: mmalenko
// (c) 2018 NIC SPbETU
// */

#include <theodolite/logic/memory/memory_interface.h>
#include <theodolite/logic/sys_config/sys_tester_interface.h>

MemoryInterface* getEeprom()
{
  return NULL;
}

MemoryInterface *getFlash()
{
  return NULL;
}

SystemTesterInterface *getSystemTester()
{
  return NULL;
}

