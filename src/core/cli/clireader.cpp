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

#include "core/cli/clireader.hpp"

CLIReaderClass::CLIReaderClass()
{
    _inStr.reserve(200);
    _inStr = "";
    _complete = false;
}

bool CLIReaderClass::isNewString() const
{
    return _complete;
}

const String& CLIReaderClass::getString() const
{
    return _inStr;
}

void CLIReaderClass::read()
{
    while (Serial.available()) {
        char inChar = (char)Serial.read();
        Serial.print(inChar);
        if (inChar == '\n') {
            _complete = true;
            _inStr.remove(_inStr.length() - 1, 1);
        } else {
            _inStr += inChar;
        }
    }
}

void CLIReaderClass::reset()
{
    _inStr = "";
    _complete = false;
}

CLIReaderClass CLIReader;
