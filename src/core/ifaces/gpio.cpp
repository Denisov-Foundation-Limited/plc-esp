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

#include "core/ifaces/gpio.hpp"

#include "core/ext.hpp"

/*********************************************************************/
/*                                                                   */
/*                          PUBLIC FUNCTIONS                         */
/*                                                                   */
/*********************************************************************/

GPIOIface::GPIOIface(Logger *log, Extenders *ext, const String &name, uint8_t pin, GpioMode mode, GpioPull pull, ExtenderId extId)
{
    _name = name;
    _pin = pin;
    _mode = mode;
    _pull = pull;
    _extId = extId;
    _log = log;
    _ext = ext;
}

const String &GPIOIface::getName() const
{
    return _name;
}

uint8_t GPIOIface::getPin() const
{
    return _pin;
}

GpioMode GPIOIface::getMode() const
{
    return _mode;
}

GpioPull GPIOIface::getPull() const
{
    return _pull;
}

void GPIOIface::setName(const String &name)
{
    _name = name;
}

void GPIOIface::setPin(uint8_t pin)
{
    _pin = pin;
}

void GPIOIface::setPull(GpioPull pull)
{
    _pull = pull;
}

void GPIOIface::setMode(GpioMode mode)
{
    _mode = mode;
}

void GPIOIface::setExtId(ExtenderId id)
{
    _extId = id;
}

void GPIOIface::write(bool val)
{
    if (_extId == EXT_NOT_USED) {
        digitalWrite(_pin, val);
    } else {
        Extender *e = nullptr;

        if ((e = _ext->getById(_extId)) == nullptr) {
            _log->error(LOG_MOD_GPIO, String(F("Extender ")) + String(_extId) + String(F(" not found")));
            return;
        }

        e->write(_pin, val);
    }
    _state = val;
}

bool GPIOIface::read()
{
    if (_extId == EXT_NOT_USED) {
        return (digitalRead(_pin) == HIGH) ? true : false;
    } else {
        Extender *e = nullptr;

        if ((e = _ext->getById(_extId)) == nullptr) {
            _log->error(LOG_MOD_GPIO, String(F("Extender ")) + String(_extId) + String(F(" not found")));
            return false;
        }

        return e->read(_pin);
    }
}

bool GPIOIface::getState()
{
    if (_mode == GPIO_MOD_INPUT) {
        return read();
    } else {
        return _state;
    }
}

ExtenderId GPIOIface::getExtId() const
{
    return _extId;
}

IntType GPIOIface::getType() const
{
    return INT_TYPE_GPIO;
}
