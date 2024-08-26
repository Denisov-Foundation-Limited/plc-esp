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

#ifndef __CLI_CONFIGURATOR_HPP__
#define __CLI_CONFIGURATOR_HPP__

#include <Arduino.h>

#include "net/wifi.hpp"
#include "core/cli/cliinfo.hpp"
#include "utils/utils.hpp"
#include "net/wifi.hpp"

#include <vector>

class CLIConfigurator
{
private:
    Wireless    *_wifi;
    Gpio        *_gpio;
    Extenders   *_ext;
    Logger      *_log; 
public:
    CLIConfigurator(Logger *log, Wireless *wifi, Gpio *gpio, Extenders *ext);
    bool configWiFi(const String &cmd);
    bool configTanks(const String &cmd);
    bool configTank(const String &tankName, const String &cmd);
    bool configGpio(const String &cmd);
    bool configPin(const String &tankName, const String &cmd);
    bool configExts(const String &cmd);
    bool configExt(ExtenderId extId, const String &cmd);
};

#endif /* __CLI_CONFIGURATOR_HPP__*/
