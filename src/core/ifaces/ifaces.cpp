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

void InterfacesClass::addInterface(Interface *iface)
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

        case INT_TYPE_UART:
            sType = F("UART");
            break;

        case INT_TYPE_RELAY:
            sType = F("Relay");
            break;

        case INT_TYPE_DIGITAL_INPUT:
            sType = F("DigitalInput");
            break;
    }

    _ifaces.push_back(iface);
    String startLog;
    if (!iface->getExtended()) {
        startLog = String(F("Add internal interface: "));
    } else {
        startLog = String(F("Add external interface: "));
    }
    Log.info(LOG_MOD_IFACES, startLog + iface->getName() + String(F(" type: ")) + sType);
}

Interface *InterfacesClass::getInterface(const String &name)
{
    for (auto *iface : _ifaces) {
        if (iface->getName() == name) {
            return iface;
        }
    }
    return nullptr;
}

const std::vector<Interface *> &InterfacesClass::getInterfaces() const
{
    return _ifaces;
}

void InterfacesClass::getInterfacesByType(std::vector<Interface *> &ifs, IfType type)
{
    for (auto *iface : _ifaces) {
        if (iface->getType() == type) {
            ifs.push_back(iface);
        }
    }
}

InterfacesClass Interfaces;
