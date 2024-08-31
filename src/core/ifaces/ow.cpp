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

#include "core/ifaces/ow.hpp"

OneWireIface::OneWireIface(const String &name, uint8_t pin)
{
    _name = name;
    _pin = pin;
}

void OneWireIface::setPin(uint8_t gpio)
{
    _pin = gpio;
}

uint8_t OneWireIface::getPin() const
{
    return _pin;
}

const String &OneWireIface::getName() const
{
    return _name;
}

void OneWireIface::setName(const String &name)
{
    _name = name;
}

void OneWireIface::findAddresses(std::vector<String> &addrs)
{

}

IntType OneWireIface::getType() const
{
    return INT_TYPE_OW;
}
