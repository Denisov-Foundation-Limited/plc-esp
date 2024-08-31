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

#include "core/ifaces/ifaces.hpp"

Interfaces::Interfaces(Logger *log)
{
    _log = log;
}

void Interfaces::addInterface(Interface *iface)
{
    String sType;

    switch (iface->getType()) {
        case INT_TYPE_GPIO:
            sType = F("GPIO");
            break;

        case INT_TYPE_SPI:
            sType = F("SPI");
            break;

        case INT_TYPE_I2C:
            sType = F("I2C");
            break;

        case INT_TYPE_OW:
            sType = F("OneWire");
            break;
    }

    _ifaces.push_back(iface);
    _log->info(LOG_MOD_IFACES, String(F("Add interface: ")) + iface->getName() + String(F(" type: ")) + sType);
}

Interface *Interfaces::getInterface(const String &name)
{
    for (auto *iface : _ifaces) {
        if (iface->getName() == name) {
            return iface;
        }
    }
    return nullptr;
}

const std::vector<Interface *> &Interfaces::getInterfaces() const
{
    return _ifaces;
}
