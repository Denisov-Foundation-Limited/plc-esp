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
        case IF_TYPE_GPIO:
            sType = F("GPIO");
            break;

        case IF_TYPE_SPI:
            sType = F("SPI");
            break;

        case IF_TYPE_I2C:
            sType = F("I2C");
            break;

        case IF_TYPE_OW:
            sType = F("OneWire");
            break;

        case IF_TYPE_UART:
            sType = F("UART");
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

void InterfacesClass::getGpioByType(GpioType type, std::vector<Interface *> &gpio)
{
    for (auto *iface : _ifaces) {
        if (iface->getType() == IF_TYPE_GPIO) {
            auto *g = static_cast<IfGPIO *>(iface);
            if (g->getPinType() == type) {
                gpio.push_back(iface);
            }
        }
    }
}

InterfacesClass Interfaces;
