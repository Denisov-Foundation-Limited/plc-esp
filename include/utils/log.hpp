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

#ifndef __LOG_HPP__
#define __LOG_HPP__

#include <Arduino.h>

typedef enum {
    LOG_MOD_MAIN,
    LOG_MOD_CFG,
    LOG_MOD_GPIO,
    LOG_MOD_GSM,
    LOG_MOD_WIFI,
    LOG_MOD_CLI
} LogModule;

typedef enum {
    LOG_TYPE_ERROR,
    LOG_TYPE_INFO,
    LOG_TYPE_WARNING
} LogType;

class Logger
{
private:
    void logging(LogType type, LogModule mod, const String &msg);
public:
    void begin();
    void info(LogModule mod, const String &msg);
    void error(LogModule mod, const String &msg);
    void warning(LogModule mod, const String &msg);
};

#endif /* __LOG_HPP__ */
