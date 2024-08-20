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

#include "core/cli/clireader.hpp"

CLIReader::CLIReader()
{
    _inStr.reserve(200);
    _inStr = "";
    _complete = false;
}

bool CLIReader::isNewString() const
{
    return _complete;
}

const String& CLIReader::getString() const
{
    return _inStr;
}

void CLIReader::read()
{
    while (Serial.available()) {
        char inChar = (char)Serial.read();
        Serial.print(inChar);
        if (inChar == '\n') {
            _complete = true;
            _inStr[_inStr.length() - 1] = '\0';
        } else {
            _inStr += inChar;
        }
    }
}

void CLIReader::reset()
{
    _inStr = "";
    _complete = false;
}