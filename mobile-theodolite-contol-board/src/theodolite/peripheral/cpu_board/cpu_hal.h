///*
// * hal.h
// *
// *
// *      Author: mmalenko
// * (c) 2016 NIC SPbETU
// */

#ifndef INCLUDE_CPU_HAL_H_
#define INCLUDE_CPU_HAL_H_

#include "stdint.h"
#include "DSP281x_Device.h"
#include "DSP281x_Gpio.h"

/*
 * Pin descrintion
 * PWMAZ_POS     GPIOA0  PWM1
 * PWMAZ_NEG     GPIOA1  PWM2
 * PWMAL_POS     GPIOA2  PWM3
 * PWMAL_NEG     GPIOA3  PWM4
 * INPUT_EN_AZ   GPIOA4  OUTPUT/PP ACTIVE HIGH
 * INPUT_EN_AL   GPIOA5  OUTPUT/PP ACTIVE HIGH
 * BREAK_AZ      GPIOA6  OUTPUT/PP ACTIVE HIGH
 * BREAK_AL                           GPIOA7  OUTPUT/PP ACTIVE HIGH
 *
 * ENC_BOARD_TXD GPIOF4  SCI
 * ENC_BOARD_RXD GPIOF5  SCI
 *
 * CPU_TXD                           GPIOG4  SCI
 * CPU_RXD       GPIOG5  SCI
 *
 * LED_1         GPIOE2  OUTPUT/OD ACTIVE LOW
 * LED_2         GPIOF13 OUTPUT/OD ACTIVE LOW
 *
 * KEY1                                        GPIOD0  INPUT/PP
 * KEY2          GPIOD1  INPUT/PP
 * KEY3          GPIOD5  INPUT/PP
 * KEY4          GPIOD6  INPUT/PP
 * MU                                        GPIOE1  INPUT/PP
 * OFF                               GPIOF14 INPUT/PP
 *
 * SYNC_PULSE    GPIOE1  INPUT/PP XINT
 *
 * UNKNOWN
 *
 * B-F3-???
 *
 * F0-
 * FC-
 *
 * */

#define GPIO_MUX_PERIPHERAL_IO_MODE  1
#define GPIO_MUX_DIGITAL_IO_MODE     0

#define GPIO_OUTPUT_DIR              1
#define GPIO_INPUT_DIR               0

#define PWMAZ_POS_MUX       GpioMuxRegs.GPAMUX.bit.PWM1_GPIOA0
#define PWMAZ_POS_MUX_VAL   GPIO_MUX_PERIPHERAL_IO_MODE
#define PWMAZ_POS_DIR       GpioMuxRegs.GPADIR.bit.GPIOA0 //no effect in GPIO_MUX_PERIPHERAL_IO_MODE
#define PWMAZ_POS_DIR_VAL   GPIO_OUTPUT_DIR  //no effect in GPIO_MUX_PERIPHERAL_IO_MODE
#define PWMAZ_POS_PIN       GpioDataRegs.GPADAT.bit.GPIOA0

#define PWMAZ_NEG_MUX       GpioMuxRegs.GPAMUX.bit.PWM2_GPIOA1
#define PWMAZ_NEG_MUX_VAL   GPIO_MUX_PERIPHERAL_IO_MODE
#define PWMAZ_NEG_DIR       GpioMuxRegs.GPADIR.bit.GPIOA1 //no effect in GPIO_MUX_PERIPHERAL_IO_MODE
#define PWMAZ_NEG_DIR_VAL   GPIO_OUTPUT_DIR                //no effect in GPIO_MUX_PERIPHERAL_IO_MODE
#define PWMAZ_NEG_PIN       GpioDataRegs.GPADAT.bit.GPIOA1

#define PWMAL_POS_MUX       GpioMuxRegs.GPAMUX.bit.PWM3_GPIOA2
#define PWMAL_POS_MUX_VAL   GPIO_MUX_PERIPHERAL_IO_MODE
#define PWMAL_POS_DIR       GpioMuxRegs.GPADIR.bit.GPIOA2 //no effect in GPIO_MUX_PERIPHERAL_IO_MODE
#define PWMAL_POS_DIR_VAL   GPIO_OUTPUT_DIR
#define PWMAL_POS_PIN       GpioDataRegs.GPADAT.bit.GPIOA2  //no effect in GPIO_MUX_PERIPHERAL_IO_MODE

#define PWMAL_NEG_MUX       GpioMuxRegs.GPAMUX.bit.PWM4_GPIOA3
#define PWMAL_NEG_MUX_VAL   GPIO_MUX_PERIPHERAL_IO_MODE
#define PWMAL_NEG_DIR       GpioMuxRegs.GPADIR.bit.GPIOA3 //no effect in GPIO_MUX_PERIPHERAL_IO_MODE
#define PWMAL_NEG_DIR_VAL   GPIO_OUTPUT_DIR              //no effect in GPIO_MUX_PERIPHERAL_IO_MODE
#define PWMAL_NEG_PIN       GpioDataRegs.GPADAT.bit.GPIOA3

#define INPUT_EN_AZ_MUX     GpioMuxRegs.GPAMUX.bit.PWM5_GPIOA4
#define INPUT_EN_AZ_MUX_VAL GPIO_MUX_DIGITAL_IO_MODE
#define INPUT_EN_AZ_DIR     GpioMuxRegs.GPADIR.bit.GPIOA4
#define INPUT_EN_AZ_DIR_VAL GPIO_OUTPUT_DIR
#define INPUT_EN_AZ_DATA_PIN  GpioDataRegs.GPADAT.bit.GPIOA4

#define INPUT_EN_AL_MUX     GpioMuxRegs.GPAMUX.bit.PWM6_GPIOA5
#define INPUT_EN_AL_MUX_VAL GPIO_MUX_DIGITAL_IO_MODE
#define INPUT_EN_AL_DIR     GpioMuxRegs.GPADIR.bit.GPIOA5
#define INPUT_EN_AL_DIR_VAL GPIO_OUTPUT_DIR
#define INPUT_EN_AL_DATA_PIN  GpioDataRegs.GPADAT.bit.GPIOA5

#define BREAK_AZ_MUX        GpioMuxRegs.GPAMUX.bit.T1PWM_GPIOA6
#define BREAK_AZ_MUX_VAL    GPIO_MUX_DIGITAL_IO_MODE
#define BREAK_AZ_DIR        GpioMuxRegs.GPADIR.bit.GPIOA6
#define BREAK_AZ_DIR_VAL    GPIO_OUTPUT_DIR
#define BREAK_AZ_DATA_PIN   GpioDataRegs.GPADAT.bit.GPIOA6
volatile uint16_t* const AZIMUTH_BREAK_GPIO = &GpioDataRegs.GPADAT.all;
const uint16_t AZIMUTH_BREAK_GPIO_PIN = 0x40;

#define BREAK_AL_MUX        GpioMuxRegs.GPAMUX.bit.T2PWM_GPIOA7
#define BREAK_AL_MUX_VAL    GPIO_MUX_DIGITAL_IO_MODE
#define BREAK_AL_DIR        GpioMuxRegs.GPADIR.bit.GPIOA7
#define BREAK_AL_DIR_VAL    GPIO_OUTPUT_DIR
#define BREAK_AL_DATA_PIN   GpioDataRegs.GPADAT.bit.GPIOA7
volatile uint16_t* const ELEVATION_BREAK_GPIO = &GpioDataRegs.GPADAT.all;
const uint16_t ELEVATION_BREAK_GPIO_PIN = 0x80;

#define ENC_BOARD_TXD_MUX      GpioMuxRegs.GPFMUX.bit.SCITXDA_GPIOF4
#define ENC_BOARD_TXD_MUX_VAL  GPIO_MUX_PERIPHERAL_IO_MODE
#define ENC_BOARD_TXD_DIR      GpioMuxRegs.GPFDIR.bit.GPIOF4  //no effect in GPIO_MUX_PERIPHERAL_IO_MODE
#define ENC_BOARD_TXD_DIR_VAL  GPIO_OUTPUT_DIR                                             //no effect in GPIO_MUX_PERIPHERAL_IO_MODE

#define ENC_BOARD_RXD_MUX      GpioMuxRegs.GPFMUX.bit.SCIRXDA_GPIOF5
#define ENC_BOARD_RXD_MUX_VAL  GPIO_MUX_PERIPHERAL_IO_MODE
#define ENC_BOARD_RXD_DIR      GpioMuxRegs.GPFDIR.bit.GPIOF5  //no effect in GPIO_MUX_PERIPHERAL_IO_MODE
#define ENC_BOARD_RXD_DIR_VAL  GPIO_INPUT_DIR                                             //no effect in GPIO_MUX_PERIPHERAL_IO_MODE

#define ENC_BOARD_UART         SciaRegs

#define CPU_TXD_MUX                      GpioMuxRegs.GPGMUX.bit.SCITXDB_GPIOG4
#define CPU_TXD_MUX_VAL        GPIO_MUX_PERIPHERAL_IO_MODE
#define CPU_TXD_DIR            GpioMuxRegs.GPGDIR.bit.GPIOG4  //no effect in GPIO_MUX_PERIPHERAL_IO_MODE
#define CPU_TXD_DIR_VAL        GPIO_OUTPUT_DIR                                             //no effect in GPIO_MUX_PERIPHERAL_IO_MODE

#define CPU_RXD_MUX            GpioMuxRegs.GPGMUX.bit.SCIRXDB_GPIOG5
#define CPU_RXD_MUX_VAL        GPIO_MUX_PERIPHERAL_IO_MODE
#define CPU_RXD_DIR            GpioMuxRegs.GPGDIR.bit.GPIOG5  //no effect in GPIO_MUX_PERIPHERAL_IO_MODE
#define CPU_RXD_DIR_VAL        GPIO_INPUT_DIR                                             //no effect in GPIO_MUX_PERIPHERAL_IO_MODE

#define CPU_UART               ScibRegs
#define ENCODER_UART           SciaRegs

#define LED1_MUX               GpioMuxRegs.GPEMUX.bit.XNMI_XINT13_GPIOE2
#define LED1_MUX_VAL           GPIO_MUX_DIGITAL_IO_MODE
#define LED1_DIR               GpioMuxRegs.GPEDIR.bit.GPIOE2
#define LED1_DIR_VAL           GPIO_OUTPUT_DIR
#define LED1_PIN               GpioDataRegs.GPEDAT.bit.GPIOE2
#define LED1_PIN_TOGGLE        GpioDataRegs.GPETOGGLE.bit.GPIOE2
volatile uint16_t* const RECEIVE_CMD_LED_GPIO = &GpioDataRegs.GPEDAT.all;
const uint16_t RECEIVE_CMD_LED_GPIO_PIN = 0x4;

#define LED2_MUX               GpioMuxRegs.GPFMUX.bit.MDRA_GPIOF13
#define LED2_MUX_VAL           GPIO_MUX_DIGITAL_IO_MODE
#define LED2_DIR               GpioMuxRegs.GPFDIR.bit.GPIOF13
#define LED2_DIR_VAL           GPIO_OUTPUT_DIR
#define LED2_PIN               GpioDataRegs.GPFDAT.bit.GPIOF13
#define LED2_PIN_TOGGLE        GpioDataRegs.GPFTOGGLE.bit.GPIOF13
volatile uint16_t* const SEND_REPLY_LED_GPIO = &GpioDataRegs.GPFDAT.all;
const uint16_t SEND_REPLY_LED_GPIO_PIN = 0x2000;

//KEY1
#define ELEVATION_DECREMENT_ANGLE_KEY_MUX               GpioMuxRegs.GPDMUX.bit.T1CTRIP_PDPA_GPIOD0
#define ELEVATION_DECREMENT_ANGLE_KEY_MUX_VAL           GPIO_MUX_DIGITAL_IO_MODE
#define ELEVATION_DECREMENT_ANGLE_KEY_DIR               GpioMuxRegs.GPDDIR.bit.GPIOD0
#define ELEVATION_DECREMENT_ANGLE_KEY_DIR_VAL           GPIO_INPUT_DIR
volatile uint16_t* const ELEVATION_DECREMENT_KEY_GPIO = &GpioDataRegs.GPDDAT.all;
const uint16_t ELEVATION_DECREMENT_KEY_GPIO_PIN = 0x1;

//KEY2
#define ELEVATION_INCREMENT_ANGLE_KEY_MUX               GpioMuxRegs.GPDMUX.bit.T2CTRIP_SOCA_GPIOD1
#define ELEVATION_INCREMENT_ANGLE_KEY_MUX_VAL           GPIO_MUX_DIGITAL_IO_MODE
#define ELEVATION_INCREMENT_ANGLE_KEY_DIR               GpioMuxRegs.GPDDIR.bit.GPIOD1
#define ELEVATION_INCREMENT_ANGLE_KEY_DIR_VAL           GPIO_INPUT_DIR
volatile uint16_t* const ELEVATION_INCREMENT_KEY_GPIO = &GpioDataRegs.GPDDAT.all;
const uint16_t ELEVATION_INCREMENT_KEY_GPIO_PIN = 0x2;

//KEY3
#define AZIMUTH_DECREMENT_ANGLE_KEY_MUX               GpioMuxRegs.GPDMUX.bit.T3CTRIP_PDPB_GPIOD5
#define AZIMUTH_DECREMENT_ANGLE_KEY_MUX_VAL           GPIO_MUX_DIGITAL_IO_MODE
#define AZIMUTH_DECREMENT_ANGLE_KEY_DIR               GpioMuxRegs.GPDDIR.bit.GPIOD5
#define AZIMUTH_DECREMENT_ANGLE_KEY_DIR_VAL           GPIO_INPUT_DIR
volatile uint16_t* const AZIMUTH_DECREMENT_KEY_GPIO = &GpioDataRegs.GPDDAT.all;
const uint16_t AZIMUTH_DECREMENT_KEY_GPIO_PIN = 0x20;

//KEY4
#define AZIMUTH_INCREMENT_ANGLE_KEY_MUX               GpioMuxRegs.GPDMUX.bit.T4CTRIP_SOCB_GPIOD6
#define AZIMUTH_INCREMENT_ANGLE_KEY_MUX_VAL           GPIO_MUX_DIGITAL_IO_MODE
#define AZIMUTH_INCREMENT_ANGLE_KEY_DIR               GpioMuxRegs.GPDDIR.bit.GPIOD6
#define AZIMUTH_INCREMENT_ANGLE_KEY_DIR_VAL           GPIO_INPUT_DIR
volatile uint16_t* const AZIMUTH_INCREMENT_KEY_GPIO = &GpioDataRegs.GPDDAT.all;
const uint16_t AZIMUTH_INCREMENT_KEY_GPIO_PIN = 0x40;

#define MU_MUX                 GpioMuxRegs.GPEMUX.bit.XINT1_XBIO_GPIOE0
#define MU_MUX_VAL             GPIO_MUX_DIGITAL_IO_MODE
#define MU_DIR                 GpioMuxRegs.GPEDIR.bit.GPIOE0
#define MU_DIR_VAL             GPIO_INPUT_DIR
volatile uint16_t* const MU_GPIO = &GpioDataRegs.GPEDAT.all;
const uint16_t MU_GPIO_PIN = 0x1;

#define OFF_MUX                GpioMuxRegs.GPFMUX.bit.XF_GPIOF14
#define OFF_MUX_VAL            GPIO_MUX_DIGITAL_IO_MODE
#define OFF_DIR                GpioMuxRegs.GPFDIR.bit.GPIOF14
#define OFF_DIR_VAL            GPIO_INPUT_DIR
volatile uint16_t* const OFF_GPIO = &GpioDataRegs.GPFDAT.all;
const uint16_t OFF_GPIO_PIN = 0x4000;

#define SYNC_PULSE_MUX         GpioMuxRegs.GPEMUX.bit.XINT2_ADCSOC_GPIOE1
#define SYNC_PULSE_MUX_VAL     GPIO_MUX_DIGITAL_IO_MODE
#define SYNC_PULSE_DIR         GpioMuxRegs.GPEDIR.bit.GPIOE1   //no effect in GPIO_MUX_PERIPHERAL_IO_MODE
#define SYNC_PULSE_DIR_VAL     GPIO_INPUT_DIR                  //no effect in GPIO_MUX_PERIPHERAL_IO_MODE
volatile uint16_t* const SYNC_PULSE_GPIO = &GpioDataRegs.GPEDAT.all;
const uint16_t SYNC_PULSE_GPIO_PIN = 0x2;

#define FREQ_0_MUX         GpioMuxRegs.GPBMUX.bit.PWM10_GPIOB3
#define FREQ_0_MUX_VAL     GPIO_MUX_DIGITAL_IO_MODE
#define FREQ_0_DIR         GpioMuxRegs.GPBDIR.bit.GPIOB3   //no effect in GPIO_MUX_PERIPHERAL_IO_MODE
#define FREQ_0_DIR_VAL     GPIO_OUTPUT_DIR                  //no effect in GPIO_MUX_PERIPHERAL_IO_MODE
volatile uint16_t* const FREQ_0_GPIO = &GpioDataRegs.GPBDAT.all;
const uint16_t FREQ_0_GPIO_PIN = 0x8;

#define FREQ_1_MUX         GpioMuxRegs.GPBMUX.bit.PWM11_GPIOB4
#define FREQ_1_MUX_VAL     GPIO_MUX_DIGITAL_IO_MODE
#define FREQ_1_DIR         GpioMuxRegs.GPBDIR.bit.GPIOB4   //no effect in GPIO_MUX_PERIPHERAL_IO_MODE
#define FREQ_1_DIR_VAL     GPIO_OUTPUT_DIR                  //no effect in GPIO_MUX_PERIPHERAL_IO_MODE
volatile uint16_t* const FREQ_1_GPIO = &GpioDataRegs.GPBDAT.all;
const uint16_t FREQ_1_GPIO_PIN = 0x10;

#define FREQ_2_MUX         GpioMuxRegs.GPBMUX.bit.PWM12_GPIOB5
#define FREQ_2_MUX_VAL     GPIO_MUX_DIGITAL_IO_MODE
#define FREQ_2_DIR         GpioMuxRegs.GPBDIR.bit.GPIOB5   //no effect in GPIO_MUX_PERIPHERAL_IO_MODE
#define FREQ_2_DIR_VAL     GPIO_OUTPUT_DIR                  //no effect in GPIO_MUX_PERIPHERAL_IO_MODE
volatile uint16_t* const FREQ_2_GPIO = &GpioDataRegs.GPBDAT.all;
const uint16_t FREQ_2_GPIO_PIN = 0x20;

#define CAMERA_SERIAL_TX_MUX           GpioMuxRegs.GPBMUX.bit.PWM7_GPIOB0
#define CAMERA_SERIAL_TX_MUX_VAL       GPIO_MUX_DIGITAL_IO_MODE
#define CAMERA_SERIAL_TX_DIR           GpioMuxRegs.GPBDIR.bit.GPIOB0   //no effect in GPIO_MUX_PERIPHERAL_IO_MODE
#define CAMERA_SERIAL_TX_DIR_VAL       GPIO_OUTPUT_DIR                  //no effect in GPIO_MUX_PERIPHERAL_IO_MODE
#define CAMERA_SERIAL_TX_PIN           GpioDataRegs.GPBDAT.bit.GPIOB0
volatile uint16_t* const CAMERA_SERIAL_TX_GPIO = &GpioDataRegs.GPBDAT.all;
const uint16_t CAMERA_SERIAL_TX_GPIO_PIN = 0x1;

volatile uint16_t* const CONTROLLER_CONNECT_SENSE_GPIO = MU_GPIO;
const uint16_t CONTROLLER_CONNECT_SENSE_GPIO_PIN = MU_GPIO_PIN;

#define SYNC_SELECTOR_MUX         GpioMuxRegs.GPBMUX.bit.PWM8_GPIOB1
#define SYNC_SELECTOR_MUX_VAL     GPIO_MUX_DIGITAL_IO_MODE
#define SYNC_SELECTOR_DIR         GpioMuxRegs.GPBDIR.bit.GPIOB1   //no effect in GPIO_MUX_PERIPHERAL_IO_MODE
#define SYNC_SELECTOR_DIR_VAL     GPIO_OUTPUT_DIR                  //no effect in GPIO_MUX_PERIPHERAL_IO_MODE
volatile uint16_t* const SYNC_SELECTOR_GPIO = &GpioDataRegs.GPBDAT.all;
const uint16_t SYNC_SELECTOR_GPIO_PIN = 0x2;

//NOT USE PINS!!! DIFFERENT SCH
volatile uint16_t* const PC_UART_TX_EN_GPIO = &GpioDataRegs.GPBDAT.all;
const uint16_t PC_UART_TX_EN_GPIO_PIN = 0x2;
volatile uint16_t* const CAMERA_UART_TX_EN_GPIO = &GpioDataRegs.GPBDAT.all;
const uint16_t CAMERA_UART_TX_EN_GPIO_PIN = 0x2;

volatile uint16_t* const AZIMUTH_OVERCURRENT_GPIO = &GpioDataRegs.GPBDAT.all;
const uint16_t AZIMUTH_OVERCURRENT_GPIO_PIN = 0x2;
volatile uint16_t* const ELEVATION_OVERCURRENT_GPIO = &GpioDataRegs.GPBDAT.all;
const uint16_t ELEVATION_OVERCURRENT_GPIO_PIN = 0x2;

#endif /* INCLUDE_HAL_H_ */
