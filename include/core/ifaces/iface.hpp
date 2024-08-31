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
    INT_TYPE_GPIO,
    INT_TYPE_SPI,
    INT_TYPE_I2C,
    INT_TYPE_OW,
    INT_TYPE_UART
} IntType;

class Interface
{
public:
    virtual const String &getName() const = 0;
    virtual void setName(const String &name) = 0;
    virtual IntType getType() const = 0;
};

#endif /* __INTERFACE_HPP__ */
