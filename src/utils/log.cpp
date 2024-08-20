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
/*                          PRIVATE FUNCTIONS                        */
/*                                                                   */
/*********************************************************************/

void Logger::logging(LogType type, LogModule mod, const String &msg)
{
    String  sType;
    String  sMod;

    switch (type)
    {
    case LOG_TYPE_ERROR:
        sType = "ERROR";
        break;
    
    case LOG_TYPE_INFO:
        sType = "INFO";
        break;

    case LOG_TYPE_WARNING:
        sType = "WARN";
        break;
    }

    switch (mod)
    {
    case LOG_MOD_CFG:
        sMod = "CONFIGS";
        break;

    case LOG_MOD_MAIN:
        sMod = "MAIN";
        break;

    case LOG_MOD_GPIO:
        sMod = "GPIO";
        break;

    case LOG_MOD_GSM:
        sMod = "GSM";
        break;

    case LOG_MOD_WIFI:
        sMod = "WIFI";
        break;

    case LOG_MOD_CLI:
        sMod = "CLI";
        break;
    }

    Serial.print("\n[" + sType + "][" + sMod + "] " + msg);
}

/*********************************************************************/
/*                                                                   */
/*                          PUBLIC FUNCTIONS                         */
/*                                                                   */
/*********************************************************************/

void Logger::begin()
{
    Serial.begin(115200);
}

void Logger::info(LogModule mod, const String &msg)
{
    logging(LOG_TYPE_INFO, mod, msg);
}

void Logger::error(LogModule mod, const String &msg)
{
    logging(LOG_TYPE_ERROR, mod, msg);
}

void Logger::warning(LogModule mod, const String &msg)
{
    logging(LOG_TYPE_WARNING, mod, msg);
}