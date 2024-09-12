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
    LOG_MOD_CLI,
    LOG_MOD_WEB,
    LOG_MOD_TG,
    LOG_MOD_METEO,
    LOG_MOD_IFACES,
    LOG_MOD_ETH
} LogModule;

typedef enum {
    LOG_TYPE_ERROR,
    LOG_TYPE_INFO,
    LOG_TYPE_WARNING
} LogType;

class LogClass
{
public:
    void begin();
    void info(LogModule mod, const String &msg);
    void error(LogModule mod, const String &msg);
    void warning(LogModule mod, const String &msg);

private:
    void _logging(LogType type, LogModule mod, const String &msg);
};

extern LogClass Log;

#endif /* __LOG_HPP__ */
