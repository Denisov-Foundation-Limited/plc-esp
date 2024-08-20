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

#ifndef __CLI_READER_HPP__
#define __CLI_READER_HPP__

#include <Arduino.h>

class CLIReader
{
private:
    String  _inStr;
    bool    _complete;
public:
    CLIReader();
    bool isNewString() const;
    const String& getString() const;
    void read();
    void reset();
};

#endif /* __CLI_READER_HPP__ */
