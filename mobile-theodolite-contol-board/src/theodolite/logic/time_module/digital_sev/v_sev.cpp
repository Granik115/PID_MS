/* $Id: v_sev.c mmalenko  */
/*!
 * \file v_sev.c
 * \brief
 *
 * PROJ: BFT
 *
 * (c) 2015 NIC SPbETU
 * ------------------------------------------------------------------------ */

#include "v_sev.h"
#include "v_sev_logic.h"
#include "soft_timer/sw_timer.h"
#include "timersconfig.h"
#include "bft.pb.h"
#include "time_module/errors/errors.h"

void timeCodeParseTsk(void *pv);

static Timer digitalSevTimer = Timer(CLOCK_CONF_SECOND * 2);

void digitalSevCommonConfig()
{
  //digitalSevUartConfiguration();
  digitalSevTimer.set(CLOCK_CONF_SECOND * 2);
  getVSevTimeCode()->status |= TIME_SCALE_UNRELIABLE_FLAG;
}

void digitalSevTask()
{
  if (inTaskProcess())
  {
    digitalSevTimer.restart();
  }
  if (digitalSevTimer.expired())
  {
    getVSevTimeCode()->error = TIMEOUT_ERROR;
    sendError (timesync_messages_ErrorType_DIGITAL_SEV_TIMECODE_MISS_ERROR);
    digitalSevTimer.restart();
  }

}

void timeCodeParseTsk(void *pv)
{

}
