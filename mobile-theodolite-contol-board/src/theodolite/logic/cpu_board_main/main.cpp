///*
// * main.c
// * (c) 2016 NIC SPbETU
// */

#include <theodolite/logic/sys_config/sys_cfg.h>
#include "foc/foc.h"
#include "cmsis_os.h"
#include "pc_protocol/protocol_task.h"

int main(void)
{
  systemConfiguration();
  focInit();
  procotolTaskInit();
  osKernelStart();
}

