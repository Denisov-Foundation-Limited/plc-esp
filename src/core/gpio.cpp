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

#include "core/gpio.hpp"

#include "core/ext.hpp"

/*********************************************************************/
/*                                                                   */
/*                          PUBLIC FUNCTIONS                         */
/*                                                                   */
/*********************************************************************/

GpioPin::GpioPin(Logger *log, Extenders *ext, const String &name, uint8_t pin, GpioType type, GpioPull pull, ExtenderId extId)
{
    _name = name;
    _pin = pin;
    _type = type;
    _pull = pull;
    _extId = extId;
    _log = log;
    _ext = ext;
}

const String &GpioPin::getName() const
{
    return _name;
}

uint8_t GpioPin::getPin() const
{
    return _pin;
}

GpioType GpioPin::getType() const
{
    return _type;
}

GpioPull GpioPin::getPull() const
{
    return _pull;
}

void GpioPin::setName(const String &name)
{
    _name = name;
}

void GpioPin::setPin(uint8_t pin)
{
    _pin = pin;
}

void GpioPin::setPull(GpioPull pull)
{
    _pull = pull;
}

void GpioPin::setType(GpioType type)
{
    _type = type;
}

void GpioPin::setExtId(ExtenderId id)
{
    _extId = id;
}

void GpioPin::write(bool val)
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

bool GpioPin::read()
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

bool GpioPin::getState()
{
    if (_type == GPIO_TYPE_INPUT) {
        return read();
    } else {
        return _state;
    }
}

ExtenderId GpioPin::getExtId() const
{
    return _extId;
}

Gpio::Gpio(Logger *log, Extenders *ext)
{
    _log = log;
    _ext = ext;
}

void Gpio::addGpio(GpioPin *gpio)
{
    String sPull;

    switch (gpio->getPull())
    {
    case GPIO_PULL_NONE:
        sPull = "none";
        break;
    
    case GPIO_PULL_DOWN:
        sPull = "down";
        break;
    
    case GPIO_PULL_UP:
        sPull = "up";
        break;
    }

    _log->info(LOG_MOD_GPIO, "Add GPIO " + gpio->getName() +
        " pin: " + String(gpio->getPin()) +
        " type: " + ((gpio->getType() == GPIO_TYPE_INPUT) ? "input" : "output") +
        " pull: " + sPull);

    _pins.push_back(gpio);
}

GpioPin *Gpio::getPin(const String &name) const
{
    for (auto *p : _pins) {
        if (p->getName() == name) {
            return p;
        }
    }
    return nullptr;
}

const std::vector<GpioPin *>& Gpio::getPins() const
{
    return _pins;
}