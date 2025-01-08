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

#ifndef __CLI_COMMAND_PROCESSOR_HPP__
#define __CLI_COMMAND_PROCESSOR_HPP__

#include <Arduino.h>

#include "utils/configs.hpp"
#include "cliinfo.hpp"
#include "clicfg.hpp"
#include "core/ext.hpp"

typedef enum {
    CON_LEVEL_ENABLE,
    CON_LEVEL_CONFIG,
    CON_LEVEL_WIFI,
    CON_LEVEL_TANKS,
    CON_LEVEL_TANK,
    CON_LEVEL_IFACES,
    CON_LEVEL_IFACE,
    CON_LEVEL_EXTS,
    CON_LEVEL_EXT,
    CON_LEVEL_TG,
    CON_LEVEL_TG_USR,
    CON_LEVEL_WEB,
    CON_LEVEL_CTRLS,
    CON_LEVEL_METEO,
    CON_LEVEL_METEO_SENS
} ConsoleLevel;

class CLIProcessorClass
{
public:
    void begin();
    bool parse(const String &cmd);

private:
    ConsoleLevel    _level = CON_LEVEL_ENABLE;
    String          _objName = "";
    String          _objName2 = "";

    void _printCall();
    bool _parseEnableCmd(const String &cmd);
    bool _parseConfigCmd(const String &cmd);
    void _processExit();
};

extern CLIProcessorClass CLIProcessor;

#endif /* __CLI_COMMAND_PROCESSOR_HPP__ */