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

void LogClass::info(const String & mod, const String &msg)
{
    _logging(LOG_TYPE_INFO, mod, msg);
}

void LogClass::error(const String & mod, const String &msg)
{
    _logging(LOG_TYPE_ERROR, mod, msg);
}

void LogClass::warning(const String & mod, const String &msg)
{
    _logging(LOG_TYPE_WARNING, mod, msg);
}

/*********************************************************************/
/*                                                                   */
/*                          PRIVATE FUNCTIONS                        */
/*                                                                   */
/*********************************************************************/

void LogClass::_logging(LogType type, const String & mod, const String &msg)
{
    String  sType;

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

    Serial.println("[" + sType + "][" + mod + "] " + msg);
}

LogClass Log;
