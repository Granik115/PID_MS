///*(c) 2018 NIC SPbETU
// * pid_saver.h
// *
// *  Created on: 25 сент. 2018 г.
// *      Author: mmalenko
// */

#ifndef SRC_THEODOLITE_LOGIC_MEMORY_STATE_SAVER_H_
#define SRC_THEODOLITE_LOGIC_MEMORY_STATE_SAVER_H_

#include <theodolite/logic/memory/memory_interface.h>
#include <theodolite/logic/memory/state_saver_interface.h>
#include "firmware/interfaces/interfaces.h"

class StateSaver:
  public StateSaverInterface
{
public:
  theodolite_messages_State getState();
  bool storeState(theodolite_messages_State&);
  StateSaver(MemoryInterface &eeprom);
  private:
  MemoryInterface &m_eeprom;
  theodolite_messages_PidRegulatorCoeff getCoefficients(uint32_t offset);
};

#endif /* SRC_THEODOLITE_LOGIC_MEMORY_STATE_SAVER_H_ */
