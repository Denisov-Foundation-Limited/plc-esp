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

#include "core/ifaces/uart.hpp"

IfUART::IfUART(const String &name, uint8_t rx, uint8_t tx, unsigned rate, bool extended)
{
    _name = name;
    _pins[UART_PIN_RX] = rx;
    _pins[UART_PIN_TX] = tx;
    _rate = rate;
    _extended = extended;
}

void IfUART::setPin(UARTPin pin, uint8_t gpio)
{
    _pins[pin] = gpio;
}

uint8_t IfUART::getPin(UARTPin pin) const
{
    return _pins[pin];
}

bool IfUART::getExtended() const
{
    return _extended;
}

void IfUART::setExtended(bool state)
{
    _extended = state;
}

const String &IfUART::getName() const
{
    return _name;
}

unsigned IfUART::getRate() const
{
    return _rate;
}

void IfUART::setRate(unsigned rate)
{
    _rate = rate;
}

IfType IfUART::getType() const
{
    return INT_TYPE_UART;
}

void IfUART::setName(const String &name)
{
    _name = name;
}
