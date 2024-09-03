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

class ConfigsClass
{
public:
    bool begin();
    bool writeAll();
    bool eraseAll();
    bool showStartup();
    bool showRunning();

private:
    ConfigsSource _src;

    bool _initDevice();
    bool _readAll(ConfigsSource src);
    bool _printFile(const String &name);
    bool _generateRunning(JsonDocument &doc);
};

extern ConfigsClass Configs;

#endif /* __CONFIGS_HPP__ */
