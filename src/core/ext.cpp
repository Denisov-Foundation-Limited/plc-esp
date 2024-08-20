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



void Extenders::addExtender(Extender *ext)
{
    _exts.push_back(ext);
    if (ext->begin()) {
        _log->info(LOG_MOD_GPIO, "Add Extender " + String(ext->getID()) +
            " addr: " + String(ext->getAddr()));
    } else {
        _log->info(LOG_MOD_GPIO, "Failed to add Extender " + String(ext->getID()) +
            " addr: " + String(ext->getAddr()));
    }
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
