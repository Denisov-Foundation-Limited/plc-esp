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

#define CLI_BACK_CHAR   8
#define CLI_TAB_CHAR    9
#define CLI_ESC_CHAR    27

class CLIReaderClass
{
public:
    CLIReaderClass();
    bool isNewString() const;
    const String& getString() const;
    void read();
    void reset();

private:
    String  _inStr;
    bool    _complete;
};

extern CLIReaderClass CLIReader;

#endif /* __CLI_READER_HPP__ */
