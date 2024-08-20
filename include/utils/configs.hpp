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

#ifndef __CONFIGS_HPP__
#define __CONFIGS_HPP__

#include <Arduino.h>

#include "utils/log.hpp"
#include "core/ext.hpp"
#include "core/gpio.hpp"
#include "net/gsm.hpp"
#include "net/wifi.hpp"
#include "core/plc.hpp"

#define CONFIGS_BOARD_FILE  F("board.json")
#define CONFIGS_PLC_FILE    F("plc.json")

typedef enum {
    CFG_SRC_SD,
    CFG_SRC_FLASH
} ConfigsSource;

class Configs
{
private:
    Logger      *_log;
    GsmModem    *_modem;
    Extenders   *_ext;
    Gpio        *_gpio;
    Wireless    *_wifi;
    Plc         *_plc;

    ConfigsSource _src;

    bool initDevice();
    bool readAll(ConfigsSource src);
    bool readPLCConfigs(ConfigsSource src);
    bool readBoardConfigs(ConfigsSource src);
    bool saveBoardConfigs();
    bool savePLCConfigs();
public:
    Configs(Logger *log, GsmModem *modem, Extenders *ext, Gpio *gpio, Wireless *wifi, Plc *plc);
    bool begin();
    bool writeAll();
    bool eraseAll();
};

#endif /* __CONFIGS_HPP__ */
