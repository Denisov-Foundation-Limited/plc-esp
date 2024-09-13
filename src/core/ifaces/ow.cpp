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

OneWireIface::OneWireIface(const String &name, uint8_t pin, bool extended)
{
    _name = name;
    _pin = pin;
    _extended = extended;
}

void OneWireIface::setPin(uint8_t gpio)
{
    _pin = gpio;
    _bus.begin(gpio);
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

bool OneWireIface::getExtended() const
{
    return _extended;
}

void OneWireIface::setExtended(bool state)
{
    _extended = state;
}

void OneWireIface::findAddresses(std::vector<String> &addrs)
{
    uint8_t addr[8];

    if (_bus.search(addr))
    {
        do
        {
            String sOut = "";
            for (uint8_t i = 0; i < 8; i++)
            {
                if (addr[i] < 0x10) {
                    sOut += "0";
                }
                sOut += String(addr[i], HEX);
            }
            sOut.toUpperCase();
            addrs.push_back(sOut);
        } while (_bus.search(addr));
    }
}

IntType OneWireIface::getType() const
{
    return INT_TYPE_OW;
}
