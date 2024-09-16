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

#ifndef __CONTROLLER_HPP__
#define __CONTROLLER_HPP__

#include <Arduino.h>

typedef enum {
    CTRL_TYPE_METEO,
    CTRL_TYPE_SECURITY,
    CTRL_TYPE_TANK,
    CTRL_TYPE_SOCKET
} CtrlType;

class Controller
{
public:
    virtual const String &getName() const = 0;
    virtual void setName(const String &name) = 0;
    virtual bool getEnabled() const = 0;
    virtual void setEnabled(bool status) = 0;
    virtual CtrlType getType() const = 0;
    virtual void loop() = 0;
    virtual void begin() = 0;
};

#endif /* __CONTROLLER_HPP__ */
