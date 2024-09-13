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

#include "core/ext.hpp"

/*********************************************************************/
/*                                                                   */
/*                          PUBLIC FUNCTIONS                         */
/*                                                                   */
/*********************************************************************/

Extender::Extender(ExtenderId id, uint8_t addr, bool extended)
{
    _id = id;
    _addr = addr;
    _extended = extended;
}

uint8_t Extender::getAddr() const
{
    return _addr;
}

ExtenderId Extender::getID() const
{
    return _id;
}

bool Extender::begin()
{
    return _mcp.begin_I2C(_addr);
}

void Extender::setExtended(bool state)
{
    _extended = state;
}

bool Extender::getExtended() const
{
    return _extended;
}

void Extender::setPinMode(uint8_t pin, uint8_t mode)
{
    _mcp.pinMode(pin, mode);
}

void Extender::write(uint8_t pin, bool state)
{
    _mcp.digitalWrite(pin, (state == true) ? HIGH : LOW);
}

bool Extender::read(uint8_t pin)
{
    return (_mcp.digitalRead(pin) == HIGH) ? true : false;
}

void Extender::setID(ExtenderId id)
{
    _id = id;
}

void Extender::setAddr(unsigned addr)
{
    _addr = addr;
}

void ExtendersClass::addExtender(Extender *ext)
{
    if (ext->begin()) {
        Log.info(LOG_MOD_GPIO, "Add Extender " + String(ext->getID()) +
            " addr: " + String(ext->getAddr()));
    } else {
        Log.warning(LOG_MOD_GPIO, "Extender " + String(ext->getID()) +
            " addr: " + String(ext->getAddr()) + " is not connected");
    }
    _exts.push_back(ext);
}

Extender *ExtendersClass::getById(ExtenderId id) const
{
    for (auto *e : _exts) {
        if (e->getID() == id) {
            return e;
        }
    }
    return nullptr;
}

std::vector<Extender*> &ExtendersClass::getExtenders()
{
    return _exts;
}

bool ExtendersClass::isExists(ExtenderId id)
{
    for (auto *e : _exts) {
        if (e->getID() == id) {
            return true;
        }
    }
    return false;
}

unsigned ExtendersClass::getLastFreeAddr() const
{
    bool found = false;

    for (auto addr : _addrs) {
        found = false;
        for (auto *e : _exts) {
            if (e->getAddr() == addr) {
                found = true;
                break;
            }
        }
        if (!found) {
            return addr;
        }
    }

    return 0;
}

ExtendersClass Extenders;
