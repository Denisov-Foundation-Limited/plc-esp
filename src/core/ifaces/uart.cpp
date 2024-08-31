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

UARTIface::UARTIface(const String &name, uint8_t rx, uint8_t tx, uint8_t ctrl, unsigned rate)
{
    _name = name;
    _pins[UART_PIN_RX] = rx;
    _pins[UART_PIN_TX] = tx;
    _pins[UART_PIN_CTRL] = ctrl;
    _rate = rate;
}

void UARTIface::setPin(UARTPin pin, uint8_t gpio)
{
    _pins[pin] = gpio;
}

uint8_t UARTIface::getPin(UARTPin pin) const
{
    return _pins[pin];
}

const String &UARTIface::getName() const
{
    return _name;
}

unsigned UARTIface::getRate() const
{
    return _rate;
}

void UARTIface::setRate(unsigned rate)
{
    _rate = rate;
}

IntType UARTIface::getType() const
{
    return INT_TYPE_UART;
}

void UARTIface::setName(const String &name)
{
    _name = name;
}
