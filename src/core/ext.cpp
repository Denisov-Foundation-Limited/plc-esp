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

#include "core/ext.hpp"
#include "boards/boards.hpp"
#include "core/ifaces/i2c.hpp"

/*********************************************************************/
/*                                                                   */
/*                          PUBLIC FUNCTIONS                         */
/*                                                                   */
/*********************************************************************/

bool ExtendersClass::begin()
{
    for (uint8_t i = 0; i < PROF_EXT_MAX; i++) {
        auto bus = ActiveBoard.interfaces.ext[i];

        _ext[i].id = bus.id;
        _ext[i].addr = bus.addr;
        _ext[i].enabled = true;

        if (!I2C.getI2cBusById(bus.i2c, &_ext[i].i2c)) {
            Log.error(F("EXT"), "I2C id: " +String(bus.i2c)+ " not found.");
            return false;
        }
        if (_ext[i].mcp.begin_I2C(_ext[i].addr, _ext[i].i2c->wire)) {
            _ext[i].active = true;
        }
    }
    return true;
}

bool ExtendersClass::getExtenderById(uint8_t id, Extender **ext)
{
    for (size_t i = 0; i < _ext.size(); i++) {
        if (_ext[i].enabled && _ext[i].id == id) {
            *ext = &_ext[i];
            return true;
        }
    }
    return false;
}

void ExtendersClass::write(Extender *ext, uint16_t pin, bool state)
{
    ext->mcp.digitalWrite(pin, state);
}

bool ExtendersClass::read(Extender *ext, uint16_t pin)
{
    return ext->mcp.digitalRead(pin);
}

void ExtendersClass::setPinMode(Extender *ext, uint16_t pin, uint8_t mode)
{
    ext->mcp.pinMode(pin, mode);
}

void ExtendersClass::getExtenders(std::vector<Extender *> &ext)
{
    for (uint8_t i ; i < EXT_COUNT; i++) {
        if (_ext[i].enabled) {
            ext.push_back(&_ext[i]);
        }
    }
}

ExtendersClass Extenders;
