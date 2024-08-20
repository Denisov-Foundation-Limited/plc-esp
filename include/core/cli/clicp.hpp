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

typedef enum {
    CON_LEVEL_ENABLE,
    CON_LEVEL_CONFIG,
    CON_LEVEL_WIFI,
    CON_LEVEL_TANKS,
    CON_LEVEL_TANK
} ConsoleLevel;

class CLIProcessor
{
private:
    ConsoleLevel    _level = CON_LEVEL_ENABLE;
    String          _objName = "";

    Logger          *_log;
    Configs         *_cfg;
    CLIConfigurator *_cliCfg;
    CLIInformer     *_cliInfo;

    void printCall();
public:
    CLIProcessor(Logger  *log, Configs *cfg, CLIConfigurator *cliCfg, CLIInformer *cliInfo);
    void begin();
    bool parse(const String &cmd);
};

#endif /* __CLI_COMMAND_PROCESSOR_HPP__ */