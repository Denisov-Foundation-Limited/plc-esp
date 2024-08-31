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
#include <ArduinoJson.h>

#include "utils/log.hpp"
#include "core/ext.hpp"
#include "core/ifaces/ifaces.hpp"
#include "net/gsm.hpp"
#include "net/wifi.hpp"
#include "core/plc.hpp"

#define CONFIGS_STARTUP_FILE  F("/startup-config.json")

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
    Interfaces  *_ifaces;
    Wireless    *_wifi;
    Plc         *_plc;

    ConfigsSource _src;

    bool initDevice();
    bool readAll(ConfigsSource src);
    bool printFile(const String &name);
    bool generateRunning(JsonDocument &doc);
public:
    Configs(Logger *log, GsmModem *modem, Extenders *ext, Interfaces *ifaces, Wireless *wifi, Plc *plc);
    bool begin();
    bool writeAll();
    bool eraseAll();
    bool showStartup();
    bool showRunning();
};

#endif /* __CONFIGS_HPP__ */
