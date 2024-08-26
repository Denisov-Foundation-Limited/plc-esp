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

Extender::Extender(ExtenderId id, uint8_t addr)
{
    _id = id;
    _addr = addr;
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



void Extenders::addExtender(Extender *ext)
{
    if (ext->begin()) {
        _log->info(LOG_MOD_GPIO, "Add Extender " + String(ext->getID()) +
            " addr: " + String(ext->getAddr()));
    } else {
        _log->warning(LOG_MOD_GPIO, "Extender " + String(ext->getID()) +
            " addr: " + String(ext->getAddr()) + " is not online");
    }
    _exts.push_back(ext);
}

Extender *Extenders::getById(ExtenderId id) const
{
    for (auto *e : _exts) {
        if (e->getID() == id) {
            return e;
        }
    }
    return nullptr;
}

std::vector<Extender*> &Extenders::getExtenders()
{
    return _exts;
}

bool Extenders::isExists(ExtenderId id)
{
    for (auto *e : _exts) {
        if (e->getID() == id) {
            return true;
        }
    }
    return false;
}

unsigned Extenders::getLastFreeAddr() const
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
