///*(c) 2018 NIC SPbETU
// * pid_saver_interface.h
// *
// *  Created on: 25 сент. 2018 г.
// *      Author: mmalenko
// */

#ifndef SRC_THEODOLITE_LOGIC_MEMORY_STATE_SAVER_INTERFACE_H_
#define SRC_THEODOLITE_LOGIC_MEMORY_STATE_SAVER_INTERFACE_H_

#include <theodolite/logic/pc_protocol/third-party/theodolite_messages.pb.h>

class StateSaverInterface
{
public:
  virtual theodolite_messages_State getState()=0;
  virtual bool storeState(theodolite_messages_State&)=0;
};

#endif /* SRC_THEODOLITE_LOGIC_MEMORY_STATE_SAVER_INTERFACE_H_ */
