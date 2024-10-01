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

#ifndef __INTERFACE_HPP__
#define __INTERFACE_HPP__

#include <Arduino.h>

typedef enum {
    IF_TYPE_GPIO,
    IF_TYPE_SPI,
    IF_TYPE_I2C,
    IF_TYPE_OW,
    IF_TYPE_UART,
    IF_TYPE_RELAY,
    IF_TYPE_DIGITAL_INPUT
} IfType;

class Interface
{
public:
    virtual const String &getName() const = 0;
    virtual void setName(const String &name) = 0;
    virtual IfType getType() const = 0;
    virtual bool getExtended() const = 0;
    virtual void setExtended(bool state) = 0;
};

#endif /* __INTERFACE_HPP__ */
