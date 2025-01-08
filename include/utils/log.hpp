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

#ifndef __LOG_HPP__
#define __LOG_HPP__

#include <Arduino.h>

typedef enum {
    LOG_TYPE_ERROR,
    LOG_TYPE_INFO,
    LOG_TYPE_WARNING
} LogType;

class LogClass
{
public:
    void begin();
    void info(const String &mod, const String &msg);
    void error(const String &mod, const String &msg);
    void warning(const String &mod, const String &msg);

private:
    void _logging(LogType type, const String &mod, const String &msg);
};

extern LogClass Log;

#endif /* __LOG_HPP__ */
