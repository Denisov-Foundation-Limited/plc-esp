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

#include "utils/log.hpp"

/*********************************************************************/
/*                                                                   */
/*                          PUBLIC FUNCTIONS                         */
/*                                                                   */
/*********************************************************************/

void LogClass::begin()
{
    Serial.begin(115200);
}

void LogClass::info(LogModule mod, const String &msg)
{
    _logging(LOG_TYPE_INFO, mod, msg);
}

void LogClass::error(LogModule mod, const String &msg)
{
    _logging(LOG_TYPE_ERROR, mod, msg);
}

void LogClass::warning(LogModule mod, const String &msg)
{
    _logging(LOG_TYPE_WARNING, mod, msg);
}

/*********************************************************************/
/*                                                                   */
/*                          PRIVATE FUNCTIONS                        */
/*                                                                   */
/*********************************************************************/

void LogClass::_logging(LogType type, LogModule mod, const String &msg)
{
    String  sType;
    String  sMod;

    switch (type)
    {
    case LOG_TYPE_ERROR:
        sType = F("ERROR");
        break;
    
    case LOG_TYPE_INFO:
        sType = F("INFO");
        break;

    case LOG_TYPE_WARNING:
        sType = F("WARN");
        break;
    }

    switch (mod)
    {
    case LOG_MOD_CFG:
        sMod = F("CONFIGS");
        break;

    case LOG_MOD_MAIN:
        sMod = F("MAIN");
        break;

    case LOG_MOD_GPIO:
        sMod = F("GPIO");
        break;

    case LOG_MOD_GSM:
        sMod = F("GSM");
        break;

    case LOG_MOD_WIFI:
        sMod = F("WIFI");
        break;

    case LOG_MOD_CLI:
        sMod = F("CLI");
        break;

    case LOG_MOD_IFACES:
        sMod = F("IFACE");
        break;

    case LOG_MOD_TG:
        sMod = F("TGBOT");
        break;

    case LOG_MOD_API:
        sMod = F("API");
        break;

    case LOG_MOD_METEO:
        sMod = F("METEO");
        break;

    case LOG_MOD_ETH:
        sMod = F("ETHERNET");
        break;

    case LOG_MOD_CTRLS:
        sMod = F("CTRLS");
        break;

    case LOG_MOD_SOCKET:
        sMod = F("SOCKET");
        break;
    }

    Serial.println("[" + sType + "][" + sMod + "] " + msg);
}

LogClass Log;
