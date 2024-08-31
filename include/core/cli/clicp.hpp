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

#ifndef __CLI_COMMAND_PROCESSOR_HPP__
#define __CLI_COMMAND_PROCESSOR_HPP__

#include <Arduino.h>

#include "utils/configs.hpp"
#include "cliinfo.hpp"
#include "clicfg.hpp"
#include "core/ifaces/ifaces.hpp"
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
    CON_LEVEL_EXT
} ConsoleLevel;

class CLIProcessorClass
{
private:
    ConsoleLevel    _level = CON_LEVEL_ENABLE;
    String          _objName = "";

    void printCall();
    bool parseEnableCmd(const String &cmd);
    bool parseConfigCmd(const String &cmd);
    void processExit();
public:
    void begin();
    bool parse(const String &cmd);
};

extern CLIProcessorClass CLIProcessor;

#endif /* __CLI_COMMAND_PROCESSOR_HPP__ */