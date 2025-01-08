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

#include "core/ifaces/uart.hpp"

void UARTClass::setPin(UARTPin pin, GpioPin *gpio)
{
    _pins[pin] = gpio;
}

GpioPin *UARTClass::getPin(UARTPin pin) const
{
    return _pins[pin];
}

const String &UARTClass::getName() const
{
    return _name;
}

unsigned UARTClass::getRate() const
{
    return _rate;
}

void UARTClass::setRate(unsigned rate)
{
    _rate = rate;
}

void UARTClass::setName(const String &name)
{
    _name = name;
}
