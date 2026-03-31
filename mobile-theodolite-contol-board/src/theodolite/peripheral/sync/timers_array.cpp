/*
 * timers_array.cpp
 *
 *  Created on: 15 нояб. 2023 г.
 *      Author: mvm
 */
#include <time_module/hw_timers/timers_api.h>
#include "timer_channel.h"
#include "sync_channel.h"
#include "critical_section.h"

static void outSyncTimerConfiguration();
static void initPpsGnssTimer();
static void initMeasureTimers();
static void syncTimers();

static SevPeriod measuredPeriods;
static uint32_t previousSevPpsCCR;
static uint32_t previousGnssPpsCCR;
static uint32_t internalTriggerCCR;
static bool isSevProtectionEnabled;
static uint32_t sevProtectionCCR = TIM_FREQ_CONST - PROTECTION_WINDOW_WIDTH;

bool enableSSITimer()
{
  LL_TIM_EnableIT_UPDATE(TIM12);
  return true;
}

uint32_t getTriggerCCR(TimerTriggerSource currentSource)
{
  switch (currentSource)
  {
    case TimerTriggerSources_INTERNAL:
      return internalTriggerCCR;
      break;
    case TimerTriggerSources_PPS_GNSS:
      return previousGnssPpsCCR;
    case TimerTriggerSources_PPS_SEV:
      return previousSevPpsCCR;
      break;
  }
  return internalTriggerCCR;
}

SevPeriod getMeasuredPeriodes(void)
{
  return measuredPeriods;
}

static SyncChannel1 channel1 = SyncChannel1(SYNC_PULSE_WIDTH);
static SyncChannel2 channel2 = SyncChannel2(SYNC_PULSE_WIDTH);
static SyncChannel3 channel3 = SyncChannel3(SYNC_PULSE_WIDTH);
static SyncChannel4 channel4 = SyncChannel4(SYNC_PULSE_WIDTH);
static InternalSyncChannel internalPpsChannel = InternalSyncChannel(FPGA_NEGEDGE_CNT_VALUE);

const uint8_t CHANNELS_NUM = 4;
static SyncChannelInterface *syncChannelsInstances[CHANNELS_NUM] =
{&channel1, &channel2, &channel3, &channel4};

void initTimerChannel(TimerChannelConfig *config, Channels channel)
{
  SyncChannelInterface *syncChannel = NULL;
  switch (channel)
  {
    case CHANNEL1:
      syncChannel = &channel1;
      break;
    case CHANNEL2:
      syncChannel = &channel2;
      break;
    case CHANNEL3:
      syncChannel = &channel3;
      break;
    case CHANNEL4:
      syncChannel = &channel4;
      break;
    case PROTECT_PPS_SEV_CHANNEL:
      isSevProtectionEnabled = config->outputEnable;
      sevProtectionCCR = config->ccr1;
      break;
    case INTERNAL_PPS_CHANNEL:
      syncChannel = &internalPpsChannel;
      break;
    default:
      break;
  }
  if (!syncChannel)
    return;
  syncChannel->setConfiguration(config);
}

void timerConfiguration(void)
{
  outSyncTimerConfiguration();
  initPpsGnssTimer();
  initMeasureTimers();
  syncTimers();
}

static void syncTimers()
{
  criticalSectionBegin();
  //TODO рассмотреть синхронизацию через триггер.
  TIM2->CNT = 0;
  TIM5->CNT = 0;
  criticalSectionEnd();
}

static void initPpsGnssTimer()
{
  LL_TIM_InitTypeDef TIM_InitStruct =
  {0};
  LL_GPIO_InitTypeDef GPIO_InitStruct =
  {0};
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM3);
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);

  GPIO_InitStruct.Pin = GNSS_PPS_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_2;
  LL_GPIO_Init(GNSS_PPS_GPIO_Port, &GPIO_InitStruct);

  /* TIM3 interrupt Init */
  NVIC_SetPriority(TIM3_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
  NVIC_EnableIRQ(TIM3_IRQn);

  TIM_InitStruct.Prescaler = 0;
  TIM_InitStruct.CounterMode = LL_TIM_COUNTERMODE_UP;
  TIM_InitStruct.Autoreload = 65535;
  TIM_InitStruct.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
  LL_TIM_Init(TIM3, &TIM_InitStruct);
  LL_TIM_DisableARRPreload(TIM3);
  LL_TIM_SetClockSource(TIM3, LL_TIM_CLOCKSOURCE_INTERNAL);
  LL_TIM_SetTriggerInput(TIM3, LL_TIM_TS_TI1FP1);
  LL_TIM_SetSlaveMode(TIM3, LL_TIM_SLAVEMODE_RESET);
  LL_TIM_IC_SetFilter(TIM3, LL_TIM_CHANNEL_CH1, LL_TIM_IC_FILTER_FDIV1);
  LL_TIM_IC_SetPolarity(TIM3, LL_TIM_CHANNEL_CH1, LL_TIM_IC_POLARITY_RISING);
  LL_TIM_DisableIT_TRIG(TIM3);
  LL_TIM_DisableDMAReq_TRIG(TIM3);
  LL_TIM_SetTriggerOutput(TIM3, LL_TIM_TRGO_RESET);
  LL_TIM_DisableMasterSlaveMode(TIM3);
  LL_TIM_IC_SetActiveInput(TIM3, LL_TIM_CHANNEL_CH1, LL_TIM_ACTIVEINPUT_DIRECTTI);
  LL_TIM_IC_SetPrescaler(TIM3, LL_TIM_CHANNEL_CH1, LL_TIM_ICPSC_DIV1);
  LL_TIM_IC_SetActiveInput(TIM3, LL_TIM_CHANNEL_CH3, LL_TIM_ACTIVEINPUT_DIRECTTI);
  LL_TIM_IC_SetPrescaler(TIM3, LL_TIM_CHANNEL_CH3, LL_TIM_ICPSC_DIV1);
  LL_TIM_IC_SetFilter(TIM3, LL_TIM_CHANNEL_CH3, LL_TIM_IC_FILTER_FDIV1);
  LL_TIM_IC_SetPolarity(TIM3, LL_TIM_CHANNEL_CH3, LL_TIM_IC_POLARITY_FALLING);
  /* USER CODE BEGIN TIM3_Init 2 */
  LL_TIM_CC_EnableChannel(TIM3, LL_TIM_CHANNEL_CH3);
  LL_TIM_EnableIT_CC3(TIM3);
  LL_TIM_EnableIT_TRIG(TIM3);
  LL_TIM_EnableCounter(TIM3);
  /* USER CODE END TIM3_Init 2 */
}

static void initMeasureTimers()
{
  LL_TIM_InitTypeDef TIM_InitStruct =
  {0};
  LL_GPIO_InitTypeDef GPIO_InitStruct =
  {0};
  /* Peripheral clock enable */
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM5);
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
  /**TIM5 GPIO Configuration
   PPS_SEV   ------> TIM5_CH1
   */
  GPIO_InitStruct.Pin = SEV_PPS_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_2;
  LL_GPIO_Init(SEV_PPS_GPIO_Port, &GPIO_InitStruct);

  /* TIM5 interrupt Init */
  NVIC_SetPriority(TIM5_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
  NVIC_EnableIRQ(TIM5_IRQn);

  TIM_InitStruct.Prescaler = 0;
  TIM_InitStruct.CounterMode = LL_TIM_COUNTERMODE_UP;
  TIM_InitStruct.Autoreload = SYNC_TIMERS_PERIOD - 1;
  TIM_InitStruct.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
  LL_TIM_Init(TIM5, &TIM_InitStruct);
  LL_TIM_DisableARRPreload(TIM5);
  LL_TIM_SetClockSource(TIM5, LL_TIM_CLOCKSOURCE_INTERNAL);
  LL_TIM_SetTriggerInput(TIM5, LL_TIM_TS_ITR1);
  LL_TIM_SetSlaveMode(TIM5, LL_TIM_SLAVEMODE_DISABLED);
  LL_TIM_DisableIT_TRIG(TIM5);
  LL_TIM_DisableDMAReq_TRIG(TIM5);
  LL_TIM_SetTriggerOutput(TIM5, LL_TIM_TRGO_RESET);
  //LL_TIM_DisableMasterSlaveMode(TIM5);
  LL_TIM_EnableMasterSlaveMode(TIM5);
  LL_TIM_IC_SetActiveInput(TIM5, LL_TIM_CHANNEL_CH1, LL_TIM_ACTIVEINPUT_DIRECTTI);
  LL_TIM_IC_SetPrescaler(TIM5, LL_TIM_CHANNEL_CH1, LL_TIM_ICPSC_DIV1);
  LL_TIM_IC_SetFilter(TIM5, LL_TIM_CHANNEL_CH1, LL_TIM_IC_FILTER_FDIV1);
  LL_TIM_IC_SetPolarity(TIM5, LL_TIM_CHANNEL_CH1, LL_TIM_IC_POLARITY_FALLING);
  LL_TIM_IC_SetActiveInput(TIM5, LL_TIM_CHANNEL_CH2, LL_TIM_ACTIVEINPUT_TRC);
  LL_TIM_IC_SetPrescaler(TIM5, LL_TIM_CHANNEL_CH2, LL_TIM_ICPSC_DIV1);
  LL_TIM_IC_SetFilter(TIM5, LL_TIM_CHANNEL_CH2, LL_TIM_IC_FILTER_FDIV1);
  LL_TIM_IC_SetPolarity(TIM5, LL_TIM_CHANNEL_CH2, LL_TIM_IC_POLARITY_RISING);
  /* USER CODE BEGIN TIM5_Init 2 */
  LL_TIM_CC_EnableChannel(TIM5, LL_TIM_CHANNEL_CH1);
  LL_TIM_CC_EnableChannel(TIM5, LL_TIM_CHANNEL_CH2);

  LL_TIM_OC_InitTypeDef TIM_OC_InitStruct =
  {0};
  TIM_OC_InitStruct.OCMode = LL_TIM_OCMODE_FROZEN;
  TIM_OC_InitStruct.OCState = LL_TIM_OCSTATE_DISABLE;
  TIM_OC_InitStruct.OCNState = LL_TIM_OCSTATE_DISABLE;
  TIM_OC_InitStruct.CompareValue = SYNC_PULSE_WIDTH;
  TIM_OC_InitStruct.OCPolarity = LL_TIM_OCPOLARITY_HIGH;
  TIM_OC_InitStruct.OCIdleState = LL_TIM_OCIDLESTATE_LOW;
  LL_TIM_OC_Init(TIM5, LL_TIM_CHANNEL_CH4, &TIM_OC_InitStruct);

  LL_TIM_CC_EnableChannel(TIM5, LL_TIM_CHANNEL_CH4);

  LL_TIM_EnableIT_CC1(TIM5);
  LL_TIM_EnableIT_CC2(TIM5);
  LL_TIM_EnableIT_CC3(TIM5);
  LL_TIM_EnableIT_CC4(TIM5);
  LL_TIM_EnableIT_TRIG(TIM5);
  LL_TIM_EnableCounter(TIM5);
  /* USER CODE END TIM5_Init 2 */

}

//TIM2
static void outSyncTimerConfiguration()
{
  LL_TIM_InitTypeDef TIM_InitStruct =
  {0};
  LL_TIM_OC_InitTypeDef TIM_OC_InitStruct =
  {0};
  LL_GPIO_InitTypeDef GPIO_InitStruct =
  {0};
  /* Peripheral clock enable */
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM2);
  /* TIM2 interrupt Init */
  NVIC_SetPriority(TIM2_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
  NVIC_EnableIRQ(TIM2_IRQn);

  TIM_InitStruct.Prescaler = 0;
  TIM_InitStruct.CounterMode = LL_TIM_COUNTERMODE_UP;
  TIM_InitStruct.Autoreload = SYNC_TIMERS_PERIOD - 1;
  TIM_InitStruct.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
  LL_TIM_Init(TIM2, &TIM_InitStruct);
  LL_TIM_DisableARRPreload(TIM2);
  LL_TIM_CC_DisablePreload(TIM2);
  LL_TIM_SetClockSource(TIM2, LL_TIM_CLOCKSOURCE_INTERNAL);
  TIM_OC_InitStruct.OCMode = LL_TIM_OCMODE_PWM1;
  TIM_OC_InitStruct.OCState = LL_TIM_OCSTATE_ENABLE;
  TIM_OC_InitStruct.OCNState = LL_TIM_OCSTATE_DISABLE;
  TIM_OC_InitStruct.CompareValue = SYNC_PULSE_WIDTH;
  TIM_OC_InitStruct.OCPolarity = LL_TIM_OCPOLARITY_HIGH;
  TIM_OC_InitStruct.OCIdleState = LL_TIM_OCIDLESTATE_LOW;
  LL_TIM_OC_Init(TIM2, LL_TIM_CHANNEL_CH1, &TIM_OC_InitStruct);
  //LL_TIM_OC_DisableFast(TIM2, LL_TIM_CHANNEL_CH1);
  LL_TIM_OC_Init(TIM2, LL_TIM_CHANNEL_CH2, &TIM_OC_InitStruct);
  // LL_TIM_OC_DisableFast(TIM2, LL_TIM_CHANNEL_CH2);
  LL_TIM_OC_Init(TIM2, LL_TIM_CHANNEL_CH3, &TIM_OC_InitStruct);
  //LL_TIM_OC_DisableFast(TIM2, LL_TIM_CHANNEL_CH3);
  LL_TIM_OC_Init(TIM2, LL_TIM_CHANNEL_CH4, &TIM_OC_InitStruct);
//  LL_TIM_OC_DisableFast(TIM2, LL_TIM_CHANNEL_CH4);
  LL_TIM_SetTriggerOutput(TIM2, LL_TIM_TRGO_RESET);
  LL_TIM_DisableMasterSlaveMode(TIM2);

  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);
  /**TIM2 GPIO Configuration
   PA3   ------> TIM2_CH4
   PB10   ------> TIM2_CH3
   PA15   ------> TIM2_CH1
   PB3   ------> TIM2_CH2
   */
  // GPIO_InitStruct.Pin = SYNC_CH4_Pin | SYNC_CH1_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_1;
  LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  // GPIO_InitStruct.Pin = SYNC_CH3_Pin | SYNC_CH2_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_1;
  LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  LL_TIM_CC_EnableChannel(TIM2, LL_TIM_CHANNEL_CH1);
  LL_TIM_CC_EnableChannel(TIM2, LL_TIM_CHANNEL_CH2);
  LL_TIM_CC_EnableChannel(TIM2, LL_TIM_CHANNEL_CH3);
  LL_TIM_CC_EnableChannel(TIM2, LL_TIM_CHANNEL_CH4);

  LL_TIM_EnableIT_CC1(TIM2);
  LL_TIM_EnableIT_CC2(TIM2);
  LL_TIM_EnableIT_CC3(TIM2);
  LL_TIM_EnableIT_CC4(TIM2);
  LL_TIM_IsEnabledIT_UPDATE(TIM2);
  LL_TIM_EnableCounter(TIM2);
}

static uint32_t measurePps(uint32_t ccr, uint32_t *previousCCR)
{
  uint32_t period;
  uint32_t previous = *previousCCR;
  *previousCCR = ccr;
  if (ccr >= previous)
    return ccr - previous;
  return SYNC_TIMERS_PERIOD - previous + ccr;
}

//TODO remove
static uint32_t sev_dbg_array[100];
static uint32_t sev_dbg_cnt;

static void sevPpsHandler(uint32_t ccr)
{
  measuredPeriods.ppsSevPeriod = measurePps(ccr, &previousSevPpsCCR);
  sev_dbg_array[sev_dbg_cnt] = measuredPeriods.ppsSevPeriod;
  sev_dbg_cnt = (sev_dbg_cnt + 1) % 100;
  putSymbolToRingBuffer(SEV_PPS_RECEIVE_EVENT, getSyncMessagesRingBuffer());
}

//TODO remove
static uint32_t dbg_array[100];
static uint32_t dbg_cnt;

static void gnssPpsHandler(uint32_t ccr)
{
  measuredPeriods.ppsGnssPeriod = measurePps(ccr, &previousGnssPpsCCR);
  dbg_array[dbg_cnt] = measuredPeriods.ppsGnssPeriod;
  dbg_cnt = (dbg_cnt + 1) % 100;
  putSymbolToRingBuffer(GNSS_PPS_RECEIVE_EVENT, getSyncMessagesRingBuffer());
}

/*
 extern "C"
 {
 void TIM3_IRQHandler(void)
 {
 if (LL_TIM_IsActiveFlag_TRIG(TIM3))
 {
 LL_TIM_ClearFlag_TRIG(TIM3);
 }
 if (LL_TIM_IsActiveFlag_CC3(TIM3))
 {
 putSymbolToRingBuffer(PPM_RECEIVE_EVENT, getSyncMessagesRingBuffer());
 LL_TIM_ClearFlag_CC3(TIM3);
 LL_TIM_ClearFlag_CC3OVR(TIM3);
 }
 }

 void TIM5_IRQHandler(void)
 {
 internalPpsChannel.pulseIsrHandler();
 if (LL_TIM_IsActiveFlag_TRIG(TIM5))
 {
 LL_TIM_ClearFlag_TRIG(TIM5);
 }
 if (LL_TIM_IsActiveFlag_CC1(TIM5))
 {
 sevPpsHandler(LL_TIM_IC_GetCaptureCH1(TIM5));
 if (isSevProtectionEnabled)
 {
 const uint32_t protectionDisableCCR = getTriggerCCR(TimerTriggerSources_PPS_SEV) + sevProtectionCCR;
 LL_TIM_OC_SetCompareCH4(TIM5, protectionDisableCCR);
 LL_TIM_CC_DisableChannel(TIM5, LL_TIM_CHANNEL_CH1);
 }
 LL_TIM_ClearFlag_CC1(TIM5);
 LL_TIM_ClearFlag_CC1OVR(TIM5);
 }
 if (LL_TIM_IsActiveFlag_CC2(TIM5))
 {
 gnssPpsHandler(LL_TIM_IC_GetCaptureCH2(TIM5));
 LL_TIM_ClearFlag_CC2(TIM5);
 LL_TIM_ClearFlag_CC2OVR(TIM5);
 }
 if (LL_TIM_IsActiveFlag_CC4(TIM5))
 {
 LL_TIM_CC_EnableChannel(TIM5, LL_TIM_CHANNEL_CH1);
 LL_TIM_ClearFlag_CC4(TIM5);
 LL_TIM_ClearFlag_CC4OVR(TIM5);
 }
 }

 void TIM2_IRQHandler(void)
 {
 for (uint8_t i = 0; i < CHANNELS_NUM; i++)
 syncChannelsInstances[i]->pulseIsrHandler();

 if (LL_TIM_IsActiveFlag_UPDATE(TIM2))
 {
 LL_TIM_ClearFlag_UPDATE(TIM2);
 }
 }
 }
 */
