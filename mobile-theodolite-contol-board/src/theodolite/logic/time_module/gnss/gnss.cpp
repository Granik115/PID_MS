/* $Id: gnss.c dpshenichnov  */
/*!
 * \file gnss.c
 * \brief
 *
 * PROJ: BFT
 *
 * (c) 2015 NIC SPbETU
 * ------------------------------------------------------------------------ */

#include "gnss.h"
#include "time_module/synchronization/sync.h"
#include "time_module/errors/errors.h"
#include "gpio.h"
#include "critical_section.h"

static GnssConfigStruct initialStruct;
static nmeap_rmc_t *rmc = NULL;
static nmeap_gga_t *g_gga = NULL;
static nmeap_context_t nmea; /* parser context */
static Timer gnssTimer = Timer(0);
static Timer gnssResetTimer = Timer(0);

nmeap_rmc_t* getGnssRmcStruct(void)
{
  return rmc;
}

nmeap_gga_t* getGnssGgaStruct(void)
{
  return g_gga;
}

static void rmc_callout(nmeap_context_t *context, void *data, void *user_data)
{
  rmc = (nmeap_rmc_t*)data;
  if (rmc->warn == RMC_UNRELIABLE_FLAG)
  {
    sendError(timesync_messages_ErrorType_GNSS_SOLUTION_UNRELIABLE_ERROR);
  }
}

static void gga_callout(nmeap_context_t *context, void *data, void *user_data)
{
  g_gga = (nmeap_gga_t*)data;
}

static int nmeap_gnrmc(nmeap_context_t *context, nmeap_sentence_t *sentence)
{
  nmeap_gprmc(context, sentence);
  return NMEAP_GNRMC;
}

static int nmeap_glrmc(nmeap_context_t *context, nmeap_sentence_t *sentence)
{
  nmeap_gprmc(context, sentence);
  return NMEAP_GLRMC;
}

static int nmeap_gngga(nmeap_context_t *context, nmeap_sentence_t *sentence)
{
  nmeap_gpgga(context, sentence);
  return NMEAP_GNGGA;
}

static int nmeap_glgga(nmeap_context_t *context, nmeap_sentence_t *sentence)
{
  nmeap_gpgga(context, sentence);
  return NMEAP_GLGGA;
}

void nmeaParserInit(void)
{
  static nmeap_rmc_t rmc; /* this is where the data from RMC messages will show up */
  static nmeap_gga_t gga;
  static int user_data; /* user can pass in anything. typically it will be a pointer to some user data */
  nmeap_init(&nmea, (void*)&user_data);
  nmeap_addParser(&nmea, "GPRMC", nmeap_gprmc, rmc_callout, &rmc);
  nmeap_addParser(&nmea, "GNRMC", nmeap_gnrmc, rmc_callout, &rmc);
  nmeap_addParser(&nmea, "GLRMC", nmeap_glrmc, rmc_callout, &rmc);
  nmeap_addParser(&nmea, "GLGGA", nmeap_glgga, gga_callout, &gga);
  nmeap_addParser(&nmea, "GNGGA", nmeap_gngga, gga_callout, &gga);
  nmeap_addParser(&nmea, "GPGGA", nmeap_gpgga, gga_callout, &gga);
}

static void initGnss(void)
{
  initialStruct.uart0Boudrate = SYNC_CODE_BAUDRATE;
  initialStruct.uart0Protocol = NMEA;
  initialStruct.uart1Boudrate = BINARY_PORT_BAUDRATE;
  initialStruct.uart1Protocol = MNP_BINARY;
  initialStruct.GLO_TIME = 1;
  initialStruct.SYS_TIME = 0;
  initialStruct.CH0_RMC_ENABLE = 1;
  initialStruct.CH0_GGA_ENABLE = 1;
  initialStruct.source = MNP_BINARY_CMD_RAM;
  gnssMkUartConfiguration(initialStruct.uart1Boudrate, GNSS_MNP_BINARY_REMAP);
  nmeaParserInit();
  timer_set(&gnssTimer, CLOCK_CONF_SECOND + CLOCK_CONF_SECOND / 3);
  timer_set(&gnssResetTimer, 10);
}

static void sendDataToBinaryPort()
{
  while (getRingBufferCount(getGnssMnpBinaryTxRingBuffer()))
  {
    mnpBinaryUartSendByteBlocking(
                                  getSymbolFromRingBuffer(
                                                          getGnssMnpBinaryTxRingBuffer()));
  }
  while (isMnpBinaryUartTxBusy())
  {

  }
}

int32_t nmeaParser(void)
{
  int32_t status = 0;
  if (getRingBufferCount(getGnssRxRingBuffer()))
  {
    status = nmeap_parse(&nmea,
                         getSymbolFromRingBuffer(getGnssRxRingBuffer()));
  }
  return status;
}

uint8_t ledState = 0;

void gnssTask(void)
{
  static time_t oldGnssTime = 0;
  static GnssTaskState state = GNSS_INIT;
  static uint32_t gnssInitAttempt = 0;
  switch (state)
  {
    case GNSS_INIT:
      enterRegion(MAIN_PROCESS);
      initGnss();
      setGpioPinLevel(GpioPins_GNSS_RST, GpioLevel_Low);
      timer_restart(&gnssResetTimer);
      state = GNSS_RESET;
      break;
    case GNSS_RESET:
      if (timer_expired(&gnssResetTimer))
      {
        setGpioPinLevel(GpioPins_GNSS_RST, GpioLevel_High);
        state = GNSS_START;
        timer_set(&gnssResetTimer, 1000);
      }
      break;
    case GNSS_START:
      if (timer_expired(&gnssResetTimer))
        state = GNSS_SEND_REQUEST;
      break;
    case GNSS_SEND_REQUEST:
      mnpSendReadConfigurationRequest(initialStruct.source);
      sendDataToBinaryPort();
      clearRingBuffer(getGnssRxRingBuffer());
      state = GNSS_WAIT_REPLY;
      break;
    case GNSS_WAIT_REPLY:
      {
      uint32_t gnssConfigWords[MNP_CONFIG_WORDS_NUMBER];
      if (mnpReadConfigurationData(gnssConfigWords))
      {
        gnssConfigurationSetting(&initialStruct, gnssConfigWords);
        sendDataToBinaryPort();
        gnssMkUartConfiguration(initialStruct.uart0Boudrate,
                                GNSS_NMEA_AND_SYNC_OUT_REMAP);
        clearRingBuffer(getGnssRxRingBuffer());
        timer_restart(&gnssTimer);
        leaveRegion(MAIN_PROCESS);
        initSyncProcess();
        state = GNSS_RUNNING;
        break;
      }
      else if (gnssInitAttempt > GNSS_INIT_ATTEMPT_MAX)
      {
        gnssMkUartConfiguration(SYNC_CODE_BAUDRATE,
                                GNSS_NMEA_AND_SYNC_OUT_REMAP);
        clearRingBuffer(getGnssRxRingBuffer());
        initSyncProcess();
        state = GNSS_ERROR;
        break;
      }
      if (timer_expired(&gnssTimer))
      {
        gnssInitAttempt++;
        timer_restart(&gnssTimer);
        state = GNSS_SEND_REQUEST;
      }
    }
      break;
    case GNSS_RUNNING:
      {
      int32_t status = 0;
      status = nmeaParser();
      switch (status)
      {
        case NMEAP_GNRMC:
          case NMEAP_GLRMC:
          {
          timer_restart(&gnssTimer);
          const time_t gnssTime = getGnssTimeSec();
          if (oldGnssTime + 1 != gnssTime)
          {
            sendError(timesync_messages_ErrorType_GNSS_SOLUTION_UNRELIABLE_ERROR);
            rmc->warn = 'V';      //unreliable
          }
          oldGnssTime = gnssTime;
          break;
        }
        case NMEAP_GPRMC:
          sendError(timesync_messages_ErrorType_GNSS_GLONASS_MISS_ERROR);
          timer_restart(&gnssTimer);
          rmc->warn = 'V';      //unreliable
          break;
        default:
          break;
      }
      if (timer_expired(&gnssTimer))
      {
        sendError(timesync_messages_ErrorType_GNSS_MESSAGE_MISS_ERROR);
        timer_restart(&gnssTimer);
        rmc->warn = 'V';      //unreliable
      }
    }
      break;
    case GNSS_ERROR:
      break;
  }
}

void convertRmcTime(nmeap_rmc_t *rmc, struct tm *time)
{
  time->tm_mday = rmc->date / 10000;
  time->tm_year = 100 + rmc->date % 100; //RMC year current 16 time.h year from 1900
  time->tm_mon = ((rmc->date % 10000) / 100) - 1; // month 0:11 in time.h RMC 1:12;
  time->tm_hour = rmc->time / 10000;
  time->tm_min = (rmc->time % 10000) / 100;
  time->tm_sec = rmc->time % 100;
  time->tm_isdst = -1;

}

time_t getGnssTimeSec(void)
{
  struct tm timeS =
  {};
  if (rmc != NULL)
  {
    convertRmcTime(rmc, &timeS);
  }
  return mktime(&timeS);
}

void setGnssTime(nmeap_rmc_t *rmc, time_t *gnssTimeSec)
{

}

