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
#include "core/ifaces/ifaces.hpp"

#include <vector>

class CLIConfiguratorClass
{
public:
    bool configWiFi(const String &cmd);
    bool configTanks(const String &cmd);
    bool configTank(const String &tankName, const String &cmd);
    bool configInterfaces(const String &cmd);
    bool configInterface(const String &ifaceName, const String &cmd);
    bool configExts(const String &cmd);
    bool configExt(ExtenderId extId, const String &cmd);
};

extern CLIConfiguratorClass CLIConfigurator;

#endif /* __CLI_CONFIGURATOR_HPP__*/
