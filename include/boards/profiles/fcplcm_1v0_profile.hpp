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

unsigned mac_address[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xFF };

#define GSM_MODEM_RATE      115200
#define LOG_SERIAL_RATE     115200
#define RS485_TRANSFER_RATE 9600

#define BUZZER_PIN      3
#define ALARM_LED_PIN   35
#define STATUS_LED_PIN  48
#define WIFI_LED_PIN    38

#define I2C_SDA_PIN 4
#define I2C_SCL_PIN 5

#define SPI_MISO_PIN    12
#define SPI_MOSI_PIN    11
#define SPI_SCK_PIN     13
#define SPI_SS_PIN      10
#define SPI_FREQ_MHZ    25

#define ETH_IRQ_PIN 1

#define OW_SECURITY_PIN     19
#define OW_TEMPERATURE_PIN  20

#define GSM_RX_PIN  18
#define GSM_TX_PIN  17

#define RS485_RX_PIN    47
#define RS485_TX_PIN    21
#define RS485_IO_PIN    14

#define RELAY_1_PIN     4
#define RELAY_2_PIN     5
#define RELAY_3_PIN     6
#define RELAY_4_PIN     7
#define RELAY_5_PIN     15
#define RELAY_6_PIN     16

#define INPUT_1_PIN     36
#define INPUT_2_PIN     37
#define INPUT_3_PIN     39
#define INPUT_4_PIN     40
#define INPUT_5_PIN     41
#define INPUT_6_PIN     42
#define INPUT_7_PIN     2
#define INPUT_8_PIN     46

#define SENSOR_1_PIN    10

#define RELAYS_COUNT    6
#define INPUTS_COUNT    8
#define SENSORS_COUNT   0

unsigned inputs[INPUTS_COUNT] = { 
    INPUT_1_PIN,
    INPUT_2_PIN,
    INPUT_3_PIN,
    INPUT_4_PIN,
    INPUT_6_PIN,
    INPUT_7_PIN,
    INPUT_8_PIN
};

unsigned relays[RELAYS_COUNT] = { 
    RELAY_1_PIN,
    RELAY_2_PIN,
    RELAY_3_PIN,
    RELAY_4_PIN,
    RELAY_5_PIN,
    RELAY_6_PIN
};

unsigned sensors[SENSORS_COUNT] =  {
    //SENSOR_1_PIN
};

#endif /* __FCPLCM_1V0_HPP__ */
