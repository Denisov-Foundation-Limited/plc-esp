/**********************************************************************/
/*                                                                    */
/* Programmable Logic Controller for ESP microcontrollers             */
/*                                                                    */
/* Copyright (C) 2024 Denisov Foundation Limited                      */
/* License: GPLv3                                                     */
/* Written by Sergey Denisov aka LittleBuster                         */
/* Email: DenisovFoundationLtd@gmail.com                              */
/*                                                                    */
/**********************************************************************/

#ifndef __FCPLCM_1V0_HPP__
#define __FCPLCM_1V0_HPP__

#include "profile.hpp"

#define GSM_MODEM_RATE      115200
#define LOG_SERIAL_RATE     115200
#define RS485_TRANSFER_RATE 9600

#define I2C_SDA_PIN 4
#define I2C_SCL_PIN 5

#define SPI_MISO_PIN    10//D6
#define SPI_MOSI_PIN    10//D7
#define SPI_SCK_PIN     10//D5
#define SPI_SS_PIN      10//D8

#define OW_1_PIN    6
#define OW_2_PIN    7

#define GSM_RX_PIN  10
#define GSM_TX_PIN  10
#define GSM_RST_PIN 9

#define RS485_RX_PIN    10
#define RS485_TX_PIN    10
#define RS485_IO_PIN    10

#define RELAY_1_PIN     10
#define RELAY_2_PIN     10
#define RELAY_3_PIN     10
#define RELAY_4_PIN     10

#define INPUT_1_PIN     10
#define INPUT_2_PIN     10
#define INPUT_3_PIN     10
#define INPUT_4_PIN     10

#define EXT_RELAY_1_PIN 0
#define EXT_RELAY_2_PIN 1
#define EXT_RELAY_3_PIN 2
#define EXT_RELAY_4_PIN 3
#define EXT_RELAY_5_PIN 4
#define EXT_RELAY_6_PIN 5
#define EXT_RELAY_7_PIN 6
#define EXT_RELAY_8_PIN 7

#define EXT_INPUT_1_PIN 8
#define EXT_INPUT_2_PIN 9
#define EXT_INPUT_3_PIN 10
#define EXT_INPUT_4_PIN 11
#define EXT_INPUT_5_PIN 12
#define EXT_INPUT_6_PIN 13
#define EXT_INPUT_7_PIN 14
#define EXT_INPUT_8_PIN 15

#define SENSOR_1_PIN    10
#define SENSOR_2_PIN    10

#define EXTENDER_ID_1   1
#define EXTENDER_ADDR_1 0x20

#define EXT_COUNT           1
#define OW_COUNT            2
#define RS485_COUNT         1
#define RELAYS_COUNT        4
#define INPUTS_COUNT        4
#define EXT_RELAYS_COUNT    8
#define EXT_INPUTS_COUNT    8
#define SENSORS_COUNT       2

ExtData extenders[EXT_COUNT] = {
    { EXTENDER_ID_1, EXTENDER_ADDR_1 }
};

ExtGpio extInputs[EXT_INPUTS_COUNT] = {
    { EXT_INPUT_1_PIN, EXTENDER_ID_1 },
    { EXT_INPUT_2_PIN, EXTENDER_ID_1 },
    { EXT_INPUT_3_PIN, EXTENDER_ID_1 },
    { EXT_INPUT_4_PIN, EXTENDER_ID_1 },
    { EXT_INPUT_5_PIN, EXTENDER_ID_1 },
    { EXT_INPUT_6_PIN, EXTENDER_ID_1 },
    { EXT_INPUT_7_PIN, EXTENDER_ID_1 },
    { EXT_INPUT_8_PIN, EXTENDER_ID_1 }
};

ExtGpio extRelays[EXT_RELAYS_COUNT] = {
    { EXT_RELAY_1_PIN, EXTENDER_ID_1 },
    { EXT_RELAY_2_PIN, EXTENDER_ID_1 },
    { EXT_RELAY_3_PIN, EXTENDER_ID_1 },
    { EXT_RELAY_4_PIN, EXTENDER_ID_1 },
    { EXT_RELAY_5_PIN, EXTENDER_ID_1 },
    { EXT_RELAY_6_PIN, EXTENDER_ID_1 },
    { EXT_RELAY_7_PIN, EXTENDER_ID_1 },
    { EXT_RELAY_8_PIN, EXTENDER_ID_1 }
};

RS485Pins rs485pins[RS485_COUNT] = {
    { RS485_RX_PIN, RS485_TX_PIN, RS485_IO_PIN }
};

unsigned ows[OW_COUNT] = {
    OW_1_PIN,
    OW_2_PIN
};

unsigned inputs[INPUTS_COUNT] = { 
    INPUT_1_PIN,
    INPUT_2_PIN,
    INPUT_3_PIN,
    INPUT_4_PIN
};

unsigned relays[RELAYS_COUNT] = { 
    RELAY_1_PIN,
    RELAY_2_PIN,
    RELAY_3_PIN,
    RELAY_4_PIN
};

unsigned sensors[SENSORS_COUNT] =  {
    SENSOR_1_PIN,
    SENSOR_2_PIN
};

#endif /* __FCPLCM_1V0_HPP__ */
