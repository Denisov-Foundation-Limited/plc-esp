/**********************************************************************/
/*                                                                    */
/* Programmable Logic Controller for ESP microcontrollers             */
/*                                                                    */
/* Copyright (C) 2024-2025 Denisov Foundation Limited                 */
/* License: GPLv3                                                     */
/* Written by Sergey Denisov aka LittleBuster                         */
/* Email: DenisovFoundationLtd@gmail.com                              */
/*                                                                    */
/**********************************************************************/

#ifndef __CLI_CONFIGURATOR_HPP__
#define __CLI_CONFIGURATOR_HPP__

#include <Arduino.h>

#include "net/core/wifi.hpp"
#include "core/cli/cliinfo.hpp"
#include "utils/utils.hpp"
#include "net/core/wifi.hpp"

#include <vector>

class CLIConfiguratorClass
{
public:
    bool configWiFi(const String &cmd);
    bool configTgBot(const String &cmd);
    bool configTgBotUser(const String &userName, const String &cmd);
};

extern CLIConfiguratorClass CLIConfigurator;

#endif /* __CLI_CONFIGURATOR_HPP__*/
