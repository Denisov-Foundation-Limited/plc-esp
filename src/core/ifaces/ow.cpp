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

#include "core/ifaces/ow.hpp"
#include "boards/boards.hpp"

bool OneWireClass::begin()
{
    for (uint8_t i = 0; i < PROF_OW_MAX; i++) {
        auto bus = ActiveBoard.interfaces.ow[i];

        _ow[i].enabled = true;
        _ow[i].pin = bus.pin;
        _ow[i].ow.begin(bus.pin);
        _ow[i].id = bus.id;

        Log.info(F("OneWire"), "OneWire id: " + String(_ow[i].id) + " inited at pin: " + String(_ow[i].pin));
    }
    return true;
}

void OneWireClass::getOWBuses(std::vector<OneWireBus *> &buses)
{
    for (uint8_t i = 0; i < _ow.size(); i++) {
        if (_ow[i].enabled) {
            buses.push_back(&_ow[i]);
        }
    }
}

bool OneWireClass::getOWBusById(uint8_t id, OneWireBus **bus)
{
    for (uint8_t i = 0; i < _ow.size(); i++) {
        if (_ow[i].id == id && _ow[i].enabled) {
            *bus = &_ow[i];
            return true;
        }
    }
    return false;
}

void OneWireClass::findDevices(OneWireBus *bus, std::vector<uint64_t> &addrs)
{
    uint8_t addr[8];
    uint64_t uAddr;

    if (bus->ow.search(addr))
    {
        do
        {
            memcpy(&uAddr, addr, sizeof(uint64_t));
            addrs.push_back(uAddr);
        } while (bus->ow.search(addr));
    }
}

OneWireClass OneWireIf;
