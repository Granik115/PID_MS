/*
 * sync.cpp
 *
 *  Created on: 11 мая 2016 г.
 *      Author: mmalenko
 * (c) 2015 NIC SPbETU
 */

#include "sync.h"
#include <time_module/hw_timers/timers_api.h>
#include "time_module/errors/errors.h"
#include "gpio.h"

static Timer sevPpsTimer = Timer(CLOCK_CONF_SECOND + CLOCK_CONF_SECOND / 2);
static Timer  sevPpmTimer = Timer(CLOCK_CONF_SECOND * 61);
static Timer  gnssPpsTimer = Timer(CLOCK_CONF_SECOND + CLOCK_CONF_SECOND / 2);
static Timer  ppmEnableTimer = Timer(CLOCK_CONF_SECOND * 59);
static RingBuffer syncMessagesBuffer =
{};
static time_t systemTimeSec, timeFromCmd;

typedef struct
{
  bool shiftMode;
  /*HZ_0_5, HZ_1, HZ_2, HZ_4, HZ_8, HZ_16*/
  timesync_messages_Frequency frequency;
  /*Переменная хранящая статус текущей синхронизации по команде*/
  SevStatus sevSettingStatus;
  /*Переменная хранящая статус текущей синхронизации*/
  SevStatus sevStatus;
  /*Значение частоты в герцах*/
  uint8_t frequencyValue;
  /*Флаг доступности ppm*/
  bool ppmAvailable;
  /*Флаг наличия pps*/
  bool ppsGnssAvailable;
  /*Флаг наличия PPS СЕВ*/
  bool ppsSevAvailable;
  /*Флаг наличия СЕВ*/
  bool analogSevAvailable;
  /*Флаг наличия СЕВ "Вешняк"*/
  bool digitalSevAvailable;
  /*Флаг наличия СЕВ Глонасс*/
  bool gnssAvailable;
  /*Период для расчета времени выдачи синхр.*/
  uint32_t internalPeriod;
  /*Период сигнала СЕВ.СЕК*/
  uint32_t sevPpsPeriod;
  /*Наличие ошибки периода сигнала СЕВ.СЕК*/
  bool sevPpsPeriodValid;
  /*Период сигнала, не используемого триггером*/
  uint32_t gnssPpsPeriod;
  bool gnssPpsPeriodValid;
  uint32_t channelQuarterShiftTime[4]; //quarter delta
  uint32_t channelShiftTime[4]; //shift with quarter delta
  uint32_t settingShiftTime[4]; //shift in command
  bool sevPpmProtectionEnableFlag;
  bool sevPpsProtectionEnableFlag;
  bool gnssPpsProtectionEnableFlag;
} SyncConfigStruct;

static SyncConfigStruct timersConfig;
static TimerChannelConfig channelsConfigArray[7];

/*Счетчики для определения наличия PPM,SEV_PPS,GNSS_PPS*/
static uint8_t ppmAvailableCnt, ppsSevAvailableCnt, ppsGnssAvailableCnt;

/*Возвращает необходимый источник синхронизации в данный момент*/
static SevStatus getNecessarySource(void);
static void internalPpsEventReceiveProcess(void);
static void channelSyncEventProcess(Channels channel);
static void channelUpdateSettingProcess(Channels channel);
static void protectionUpdateSettingProcess(Channels channel);
static void setSevProtectionState(SyncSources source, bool state);

/*API*/
void setExplicitTime(time_t time)
{
  timeFromCmd = time;
}

void syncSetFrequency(timesync_messages_Frequency frequency)
{
  timersConfig.frequency = frequency;
  switch (frequency)
  {
    case timesync_messages_Frequency_HERTZ_0_5:
      timersConfig.frequencyValue = 1;
      break;
    case timesync_messages_Frequency_HERTZ_1:
      timersConfig.frequencyValue = 1;
      break;
    case timesync_messages_Frequency_HERTZ_2:
      timersConfig.frequencyValue = 2;
      break;
    case timesync_messages_Frequency_HERTZ_4:
      timersConfig.frequencyValue = 4;
      break;
    case timesync_messages_Frequency_HERTZ_8:
      timersConfig.frequencyValue = 8;
      break;
    case timesync_messages_Frequency_HERTZ_16:
      timersConfig.frequencyValue = 16;
      break;
    default:
      timersConfig.frequencyValue = 1;
      break;
  }
}

void setSyncSource(SevStatus source)
{
  timersConfig.sevSettingStatus = source;
}

uint32_t getChannelNumber(Channels channel)
{
  uint32_t returnChannel = 0;
  switch (channel)
  {
    case CHANNEL1:
      returnChannel = 1;
      break;
    case CHANNEL2:
      returnChannel = 2;
      break;
    case CHANNEL3:
      returnChannel = 3;
      break;
    case CHANNEL4:
      returnChannel = 4;
      break;
    default:
      break;
  }
  return returnChannel;
}

void setChannelShiftValue(uint32_t channel, uint32_t shiftUs)
{
  shiftUs %= US_IN_SECOND;
  if ((channel > 0) && (channel < 5))
  {
    timersConfig.channelShiftTime[channel - 1] = shiftUs;
  }
}

void setShiftMode(bool mode)
{
  uint32_t shiftStepValue = 0;
  bool shiftEnable = mode;
  timersConfig.shiftMode = mode;
  if (shiftEnable)
  {
    if (timersConfig.frequency == timesync_messages_Frequency_HERTZ_0_5)
    {
      shiftStepValue = (2 * US_IN_SECOND)
      / (timersConfig.frequencyValue * 4);
    }
    else
    {
      shiftStepValue = US_IN_SECOND / (timersConfig.frequencyValue * 4);
    }
  }
  for (uint32_t i = 0; i < 4; i++)
  {
    timersConfig.channelQuarterShiftTime[i] = shiftStepValue * i;
    setChannelShiftValue(i + 1,
                         timersConfig.settingShiftTime[i]
                         + timersConfig.channelQuarterShiftTime[i]);
  }
}

/*channel from 1 to 4*/
void setShiftValue(uint32_t channel, uint32_t shiftUs)
{
  shiftUs %= US_IN_SECOND;
  if ((channel > 0) && (channel < 5))
  {
    timersConfig.settingShiftTime[channel - 1] = shiftUs;
  }
}

uint32_t getInternalPeriod()
{
  return timersConfig.internalPeriod;
}

/*channel from 1 to 4*/
uint32_t getShiftValue(uint32_t channel)
{
  if ((channel > 0) && (channel < 5))
  {
    return timersConfig.settingShiftTime[channel - 1];
  }
  return 0;
}

static TimerTriggerSource getTriggerSource(void)
{
  SevStatus sevStatus = getSevStatus();
  TimerTriggerSource source;
  switch (sevStatus)
  {
    case DIGITAL_SEV:
      case ANALOG_SEV:
      source = TimerTriggerSources_PPS_SEV;
      break;
    case GNSS:
      source = TimerTriggerSources_PPS_GNSS;
      break;
    case INTERNAL:
      source = TimerTriggerSources_INTERNAL;
      break;
    default:
      source = TimerTriggerSources_INTERNAL;
      break;
  }
  return source;
}

static void protectionUpdateSettingProcess(Channels channel)
{
  uint8_t channelNumber;
  bool protectionEnable;
  switch (channel)
  {
    case PROTECT_PPS_SEV_CHANNEL:
      channelNumber = SEV_PPS_PROTECTION_CHANNEL_NUMBER;
      protectionEnable = timersConfig.sevPpsProtectionEnableFlag;
      break;
    case PROTECT_PPM_SEV_CHANNEL:
      channelNumber = SEV_PPM_PROTECTION_CHANNEL_NUMBER;
      protectionEnable = timersConfig.sevPpmProtectionEnableFlag;
      break;
    case PROTECT_PPS_GNSS_CHANNEL:
      channelNumber = GNSS_PPS_PROTECTION_CHANNEL_NUMBER;
      protectionEnable = timersConfig.gnssPpsProtectionEnableFlag;
      break;
    case CHANNEL1:
      case CHANNEL2:
      case CHANNEL3:
      case CHANNEL4:
      case Channels_ALL_CHANNELS:
      return;

  }
  channelsConfigArray[channelNumber].source = getTriggerSource();
  channelsConfigArray[channelNumber].autoreload = timersConfig.internalPeriod;
  channelsConfigArray[channelNumber].ccr1 = timersConfig.internalPeriod
  - (timersConfig.internalPeriod * 3) / 1000000;
  channelsConfigArray[channelNumber].outputEnable = protectionEnable; //inversion against regular channel
  initTimerChannel(&channelsConfigArray[channelNumber], channel);
}

static void setSevProtectionState(SyncSources source, bool state)
{
  switch (source)
  {
    case SEV_PPS:
      timersConfig.sevPpsProtectionEnableFlag = state;
      break;
    case SEV_PPM:
      timersConfig.sevPpmProtectionEnableFlag = state;
      break;
    case GNSS_PPS:
      timersConfig.gnssPpsProtectionEnableFlag = state;
      break;
    default:
      break;
  }
}

void initSyncProcess(void)
{
  timerConfiguration();
  timersConfig.frequencyValue = 1;
  timersConfig.frequency = timesync_messages_Frequency_HERTZ_1;
  timersConfig.sevPpsPeriodValid = false;
  timersConfig.internalPeriod = TIM_FREQ_CONST;
  timersConfig.ppmAvailable = true;
  timersConfig.sevStatus = INTERNAL;
  setShiftMode(false);
  putSymbolToRingBuffer(CHANNEL1_UPDATE_SETTING_EVENT,
                        getSyncMessagesRingBuffer());
  putSymbolToRingBuffer(CHANNEL2_UPDATE_SETTING_EVENT,
                        getSyncMessagesRingBuffer());
  putSymbolToRingBuffer(CHANNEL3_UPDATE_SETTING_EVENT,
                        getSyncMessagesRingBuffer());
  putSymbolToRingBuffer(CHANNEL4_UPDATE_SETTING_EVENT,
                        getSyncMessagesRingBuffer());
}

RingBuffer* getSyncMessagesRingBuffer(void)
{
  return &syncMessagesBuffer;
}

SevStatus getSevStatus(void)
{
  return timersConfig.sevStatus;
}

bool isAnalogSevAlailable(void)
{
  return timersConfig.analogSevAvailable;
}

bool isDigitalSevAlailable(void)
{
  return timersConfig.digitalSevAvailable;
}
bool isGnssAlailable(void)
{
  return timersConfig.gnssAvailable;
}

bool isShiftMode(void)
{
  return timersConfig.shiftMode;
}

timesync_messages_Frequency getSyncPulseFrequency(void)
{
  return timersConfig.frequency;
}

/*Выбор источника синхронизации*/
static SevStatus getNecessarySource(void)
{
  if ((timersConfig.digitalSevAvailable)
  && (timersConfig.sevSettingStatus == DIGITAL_SEV))
  {
    return DIGITAL_SEV;
  }
  else if ((timersConfig.analogSevAvailable)
  && (timersConfig.sevSettingStatus == ANALOG_SEV))
  {
    return ANALOG_SEV;
  }
  else if ((timersConfig.gnssAvailable)
  && (timersConfig.sevSettingStatus == GNSS))
  {
    return GNSS;
  }
  else if (timersConfig.digitalSevAvailable)
  {
    return DIGITAL_SEV;
  }
  else if (timersConfig.analogSevAvailable)
  {
    return ANALOG_SEV;
  }
  else if (timersConfig.gnssAvailable)
  {
    return GNSS;
  }
  return INTERNAL;
}

//Обработка флагов
void processSyncAvailableFlag(void)
{
//Обработка статуса сев
  bool analogSevAvailableFlag = timersConfig.ppsSevAvailable
  && timersConfig.ppmAvailable && timersConfig.sevPpsPeriodValid;
  timersConfig.analogSevAvailable = analogSevAvailableFlag;
//Обработка флагов вешняка
  bool digitalSevAvailableFlag = timersConfig.ppsSevAvailable
  && (!(getVSevTimeCode()->status & TIME_SCALE_UNRELIABLE_FLAG))
  && (getVSevTimeCode()->error == NO_SEV_ERRORS)
  && timersConfig.sevPpsPeriodValid;
  timersConfig.digitalSevAvailable = digitalSevAvailableFlag;
//Обработка флагов глонасс
  const char warning = getGnssRmcStruct()->warn;
  bool gnssAvailableFlag = !((timersConfig.ppsGnssAvailable == false)
  || (warning == RMC_UNRELIABLE_FLAG)
  || (timersConfig.gnssPpsPeriodValid == false));
  timersConfig.gnssAvailable = gnssAvailableFlag;
}

static bool isPeriodValid(uint32_t oldPeriod, uint32_t period)
{
  bool isPeriodValid = (period < (oldPeriod + PERIOD_DEVIATION))
  && (period > (oldPeriod - PERIOD_DEVIATION))
  && (period < TIM_FREQ_WITH_DEVIATION_PLUS)
  && (period > TIM_FREQ_WITH_DEVIATION_MINUS);
  return isPeriodValid;
}

static void detectMissingSourceProcess(void)
{
  if (sevPpsTimer.expired())
  {
    putSymbolToRingBuffer(SEV_PPS_MISS_EVENT, &syncMessagesBuffer);
    sevPpsTimer.restart();
  }
  if (gnssPpsTimer.expired())
  {
    putSymbolToRingBuffer(GNSS_PPS_MISS_EVENT, &syncMessagesBuffer);
    gnssPpsTimer.restart();
  }
  if (sevPpmTimer.expired())
  {
    putSymbolToRingBuffer(PPM_MISS_EVENT, &syncMessagesBuffer);
    sevPpmTimer.restart();
  }
}

static void ppmMissEventProcess(void)
{
  ppmAvailableCnt = 0;
  timersConfig.ppmAvailable = false;
  sendError(timesync_messages_ErrorType_SEV_PPM_MISS_ERROR);
  setSevProtectionState(SEV_PPM, DISABLE);
}

static void ppmReceiveProcess(void)
{
  setSevProtectionState(SEV_PPM, ENABLE);
  if (ppmEnableTimer.expired())
  {
    ppmEnableTimer.restart();
    sevPpmTimer.restart();
    if (timersConfig.sevStatus == ANALOG_SEV)
    {
      struct tm *local;
      time_t currentTime = systemTimeSec;
      local = localtime(&currentTime);
      if (local->tm_sec != 0)
      {
        local->tm_sec = 0;
        systemTimeSec = mktime(local);
        systemTimeSec += 60;
      }
    }
    if (ppmAvailableCnt)
    {
      timersConfig.ppmAvailable = true;
    }
    ppmAvailableCnt = 1;
  }
}

static void sevPpsMissEventProcess(void)
{
  ppsSevAvailableCnt = 0;
  timersConfig.ppsSevAvailable = false;
  sendError(timesync_messages_ErrorType_SEV_PPS_MISS_ERROR);
  /*Если синхронизация идет от этого источника,
   * то отключать защиту нельзя,
   * т.к несколько пропусков сигнала допускаются*/
  if ((timersConfig.sevStatus != DIGITAL_SEV)
  && (timersConfig.sevStatus != ANALOG_SEV))
  {
    setSevProtectionState(SEV_PPS, DISABLE);
  }
}

uint32_t debugArray[200];
uint8_t cntDbg = 0;

static void sevPpsReceiveProcess(void)
{
  timersConfig.ppsSevAvailable = true;
  sevPpsTimer.restart();
  setSevProtectionState(SEV_PPS, ENABLE);
  uint32_t newSevPeriod = getMeasuredPeriodes().ppsSevPeriod;
  timersConfig.sevPpsPeriodValid = isPeriodValid(timersConfig.sevPpsPeriod,
                                                 newSevPeriod);
  if (!timersConfig.sevPpsPeriodValid)
  {
    setSevProtectionState(SEV_PPS, DISABLE);
    sendError(timesync_messages_ErrorType_SEV_PPS_PERIOD_ERROR);
  }
  timersConfig.sevPpsPeriod = newSevPeriod;
}

static void gnssPpsMissProcess(void)
{
  ppsGnssAvailableCnt = 0;
  timersConfig.ppsGnssAvailable = false;
  sendError(timesync_messages_ErrorType_GNSS_PPS_MISS_ERROR);
  /*Если синхронизация идет от этого источника,
   * то отключать защиту нельзя,
   * т.к несколько пропусков сигнала допускаются*/
  if (timersConfig.sevStatus != GNSS)
  {
    setSevProtectionState(GNSS_PPS, DISABLE);
  }
}

static void gnssPpsReceiveProcess(void)
{
  timersConfig.ppsGnssAvailable = true;
  setSevProtectionState(GNSS_PPS, ENABLE);
  gnssPpsTimer.restart();
  uint32_t newGnssPpsPeriod;
  newGnssPpsPeriod = getMeasuredPeriodes().ppsGnssPeriod;
  timersConfig.gnssPpsPeriodValid = isPeriodValid(timersConfig.gnssPpsPeriod,
                                                  timersConfig.gnssPpsPeriod);
  debugArray[cntDbg] = newGnssPpsPeriod;
  cntDbg = (cntDbg + 1) % 200;
  if (!timersConfig.gnssPpsPeriodValid)
  {
    setSevProtectionState(GNSS_PPS, DISABLE);
    sendError(timesync_messages_ErrorType_GNSS_PPS_PERIOD_ERROR);
  }
  timersConfig.gnssPpsPeriod = newGnssPpsPeriod;
}

static void updateInternalPpsChannel()
{
  TimerChannelConfig config;
  config.source = getTriggerSource();
  config.autoreload = timersConfig.internalPeriod;
  config.ccr1 = 0;
  initTimerChannel(&config, INTERNAL_PPS_CHANNEL);
}

/*Основной процесс синхронизации
 * */
void syncTask(void)
{
  detectMissingSourceProcess();
  //if (getRingBufferCount(&syncMessagesBuffer))
  while (getRingBufferCount(&syncMessagesBuffer))
  {
    uint8_t byte = getSymbolFromRingBuffer(&syncMessagesBuffer);
    SyncMessages message = (SyncMessages)byte;
    switch (message)
    {
      case INIT_SYNC:
        initSyncProcess();
        break;
      case PPM_RECEIVE_EVENT:
        ppmReceiveProcess();
        break;
      case SEV_PPS_RECEIVE_EVENT:
        sevPpsReceiveProcess();
        break;
      case GNSS_PPS_RECEIVE_EVENT:
        gnssPpsReceiveProcess();
        break;
      case PPM_INPUT_ENABLE_EVENT:
        break;
      case PPM_MISS_EVENT:
        ppmMissEventProcess();
        break;
      case GNSS_PPS_MISS_EVENT:
        gnssPpsMissProcess();
        break;
      case SEV_PPS_MISS_EVENT:
        sevPpsMissEventProcess();
        break;
      case INTERNAL_PPS_RECEIVE_EVENT:
        internalPpsEventReceiveProcess();
        protectionUpdateSettingProcess(PROTECT_PPS_SEV_CHANNEL);
        updateInternalPpsChannel();
        //protectionUpdateSettingProcess(PROTECT_PPM_SEV_CHANNEL);
        //protectionUpdateSettingProcess(PROTECT_PPS_GNSS_CHANNEL);
        break;
      case CHANNEL1_SYNC_EVENT:
        channelSyncEventProcess(CHANNEL1);
        break;
      case CHANNEL2_SYNC_EVENT:
        channelSyncEventProcess(CHANNEL2);
        break;
      case CHANNEL3_SYNC_EVENT:
        channelSyncEventProcess(CHANNEL3);
        break;
      case CHANNEL4_SYNC_EVENT:
        channelSyncEventProcess(CHANNEL4);
        break;
      case CHANNEL1_UPDATE_SETTING_EVENT:
        channelUpdateSettingProcess(CHANNEL1);
        break;
      case CHANNEL2_UPDATE_SETTING_EVENT:
        channelUpdateSettingProcess(CHANNEL2);
        break;
      case CHANNEL3_UPDATE_SETTING_EVENT:
        channelUpdateSettingProcess(CHANNEL3);
        break;
      case CHANNEL4_UPDATE_SETTING_EVENT:
        channelUpdateSettingProcess(CHANNEL4);
        break;
      case GNSS_PPS_INPUT_ENABLE_EVENT:
        break;
      case SEV_PPS_INPUT_ENABLE_EVENT:
        break;
    }
  }
}

typedef struct
{
  uint32_t posEdgeCounterValue;
  uint32_t nextPulseNumber;
} PulseConfig;

static PulseConfig getNearestNextSyncPulse(uint32_t frequency, uint32_t shift,
                                           uint32_t ppsCounterValue,
                                           uint32_t previousNumber)
{
  PulseConfig pulseConfig;
  uint32_t syncPulseCounterValue;
  uint32_t nextPulseNumber = (previousNumber + 1) % frequency;
  uint32_t shiftCounterValue =
  (((uint64_t)ppsCounterValue * (uint64_t)shift) / US_IN_SECOND);
  syncPulseCounterValue = ((ppsCounterValue * nextPulseNumber / frequency)
  + shiftCounterValue) % ppsCounterValue;
  pulseConfig.posEdgeCounterValue = syncPulseCounterValue;
  pulseConfig.nextPulseNumber = nextPulseNumber;
  return pulseConfig;
}

static uint32_t getSyncPulseTimeMicroSeconds(uint32_t frequency, uint32_t shiftUs,
                                             uint32_t pulseNumber)
{
  uint32_t pulseTime = (((US_IN_SECOND / frequency) * pulseNumber) + shiftUs)
  % US_IN_SECOND;
  return pulseTime;
}

static void channelSyncEventProcess(Channels channel)
{
  uint8_t channelNumber;
  TimeInterface interface;
  switch (channel)
  {
    case CHANNEL1:
      interface = UART_CHANNEL1;
      channelNumber = CHANNEL1_NUMBER;
      break;
    case CHANNEL2:
      interface = UART_CHANNEL2;
      channelNumber = CHANNEL2_NUMBER;
      break;
    case CHANNEL3:
      interface = UART_CHANNEL3;
      channelNumber = CHANNEL3_NUMBER;
      break;
    case CHANNEL4:
      interface = UART_CHANNEL4;
      channelNumber = CHANNEL4_NUMBER;
      break;
    default:
      return;
  }
  time_t syncPulseTime;

  uint8_t syncPulseInUndefinedTimeZone = (channelsConfigArray[channelNumber].ccr1
  <= (FPGA_NEGEDGE_CNT_VALUE - INTERRUPT_EXECUTE_TIME)); //т.е если событие от синхроимпульса пришло раньше чем от FPGA_PPS
  if (syncPulseInUndefinedTimeZone)
  {
    syncPulseTime = systemTimeSec + 1;
  }
  else
  {
    syncPulseTime = systemTimeSec;
  }
  if (channelsConfigArray[channelNumber].outputEnable == DISABLE)
  {
    return;
  }
  PulseTime time =
  {};
  time.mkSec = getSyncPulseTimeMicroSeconds(
                                            channelsConfigArray[channelNumber].frequencyValue,
                                            timersConfig.channelShiftTime[channelNumber],
                                            channelsConfigArray[channelNumber].pulseNumber);
  time.time = *(localtime(&syncPulseTime));
  sendTime(time, interface);
}

static bool getNextOutputState(uint8_t channelNumber)
{
  bool outputState = ENABLE;
  if (channelsConfigArray[channelNumber].frequency
  == timesync_messages_Frequency_HERTZ_0_5)
  {
    bool nextSecondIsOdd = (systemTimeSec + 1) % 2;
    if (timersConfig.shiftMode)
    {
      if ((channelNumber == CHANNEL3_NUMBER)
      || (channelNumber == CHANNEL4_NUMBER))
      {
        outputState = nextSecondIsOdd;
      }
      else
      {
        outputState = !nextSecondIsOdd;
      }
    }
    else
    {
      outputState = !nextSecondIsOdd;
    }
  }
  return outputState;
}

static void initConfigurationStruct(TimerChannelConfig *config, uint8_t channelNumber, uint32_t shift)
{
  config->source = getTriggerSource();
  config->autoreload = timersConfig.internalPeriod;
  config->frequencyValue = timersConfig.frequencyValue;
  config->frequency = timersConfig.frequency;
  config->shiftUs = shift;
  PulseConfig nextPulseConfig = getNearestNextSyncPulse(
                                                        timersConfig.frequencyValue,
                                                        config->shiftUs,
                                                        timersConfig.internalPeriod,
                                                        config->pulseNumber);
  config->ccr1 = nextPulseConfig.posEdgeCounterValue;
  config->pulseNumber = nextPulseConfig.nextPulseNumber;

  /*Если значение регистра сравнения CCR1 попадает в эту зону,
   *то при раннем приходе PPS значение счетчика CNT не достигает значения регистра CCR1 и
   импульс СИ не появится.Ставим другой режим PWM
   */
  bool pulseOverPpsJitter = config->ccr1 > TIM_FREQ_WITH_DEVIATION_MINUS;
  CompareType compareType = pulseOverPpsJitter ? CompareType_CCR_OR_SYNC : CompareType_CCR;
  config->compareType = compareType;
  config->outputEnable = getNextOutputState(channelNumber);
}

static void channelUpdateSettingProcess(Channels channel)
{
  uint8_t channelNumber;
  switch (channel)
  {
    case CHANNEL1:
      channelNumber = CHANNEL1_NUMBER;
      break;
    case CHANNEL2:
      channelNumber = CHANNEL2_NUMBER;
      break;
    case CHANNEL3:
      channelNumber = CHANNEL3_NUMBER;
      break;
    case CHANNEL4:
      channelNumber = CHANNEL4_NUMBER;
      break;
    default:
      return;
  }
  TimerChannelConfig *config = &channelsConfigArray[channelNumber];
  initConfigurationStruct(config, channelNumber, timersConfig.channelShiftTime[channelNumber]);
  initTimerChannel(config, channel);
}

/*Обработка события среза импульса FPGA_PPS
 * Время среза настроено на 5000 ед. основной шкалы(50 Мгц)
 * т.е это 100 мкс.
 * Минимальный сдвиг синхроимпульса = 50 ед.
 *FPGA_PPS синхронизирован с активным источником СЕВ
 *
 *
 *
 * */
static void internalPpsEventReceiveProcess(void)
{
  static uint8_t secToRegainSourceCnt = 0;
  processSyncAvailableFlag();
  volatile SevStatus sevStatus = getNecessarySource();

//one pulse miss logic
  if (timersConfig.sevStatus != sevStatus)
  {
    if ((sevStatus == timersConfig.sevSettingStatus)
    || (secToRegainSourceCnt >= SEC_TO_REGAIN_SOURCE))
    {
      secToRegainSourceCnt = 0;
      timersConfig.sevStatus = sevStatus;
    }
    else if (secToRegainSourceCnt < SEC_TO_REGAIN_SOURCE)
    {
      sevStatus = INTERNAL;
      secToRegainSourceCnt++;
    }
  }
  else
  {
    secToRegainSourceCnt = 0;
  }

  if ((timeFromCmd != 0) && (timersConfig.sevStatus != GNSS)
  && (timersConfig.sevStatus != DIGITAL_SEV))
  {
    systemTimeSec = timeFromCmd;
    timeFromCmd = 0;
  }
  switch (sevStatus)
  {
    case GNSS:
      {
      timersConfig.internalPeriod = timersConfig.gnssPpsPeriod;
      systemTimeSec = getGnssTimeSec();  //извлекаем время из цифрового кода
    }
      break;
    case DIGITAL_SEV:
      systemTimeSec = getVSevTimeSec();  //извлекаем время из цифрового кода
      timersConfig.internalPeriod = timersConfig.sevPpsPeriod;

      break;
    case ANALOG_SEV:
      timersConfig.internalPeriod = timersConfig.sevPpsPeriod;
      break;
    case INTERNAL:

      break;
  }
  systemTimeSec++;
  PulseTime time =
  {};
  struct tm *localTime;
  localTime = localtime(&systemTimeSec);
  time.time = *localTime;
  //setTimeIndication(&time);
  //ledIndicationTask();
}

