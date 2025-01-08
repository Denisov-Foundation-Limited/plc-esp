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

#include "core/ifaces/gpio.hpp"
#include "core/ext.hpp"
#include "boards/boards.hpp"

/*********************************************************************/
/*                                                                   */
/*                          PUBLIC FUNCTIONS                         */
/*                                                                   */
/*********************************************************************/

bool GpioClass::begin()
{
    for (uint8_t i = 0; i < PROF_GPIO_MAX; i++) {
        auto gpio = ActiveBoard.interfaces.gpio[i];

        _pins[i].enabled = true;
        _pins[i].id = gpio.id;
        _pins[i].mode = GPIO_MOD_INPUT;
        _pins[i].state = false;
        _pins[i].pin = gpio.pin;

        switch (gpio.type)
        {
            case PROF_GPIO_GENERIC:
                _pins[i].type = GPIO_TYPE_GENERIC;
                break;

            case PROF_GPIO_INPUT:
                _pins[i].type = GPIO_TYPE_INPUT;
                break;

            case PROF_GPIO_RELAY:
                _pins[i].type = GPIO_TYPE_RELAY;
                break;

            case PROF_GPIO_SENSOR:
                _pins[i].type = GPIO_TYPE_SENSOR;
                break;

            case PROF_GPIO_BUZZER:
                _pins[i].type = GPIO_TYPE_BUZZER;
                break;
        }

        if (!Extenders.getExtenderById(gpio.ext, &_pins[i].ext)) {
            Log.info(F("GPIO"), "GPIO id: " +String(_pins[i].id)+ " inited at CPU pin: " +String(_pins[i].pin));
        } else {
            Log.info(F("GPIO"), "GPIO id: " +String(_pins[i].id)+ " inited at Extender ext: " +String(_pins[i].ext->id)+" pin: " +String(_pins[i].pin));
        }

        _beginPin(&_pins[i]);
    }
    return true;
}

void GpioClass::write(GpioPin *pin, bool val)
{
    if (pin->ext == nullptr) {
        digitalWrite(pin->pin, (val == true) ? HIGH : LOW);
    } else {
        Extenders.write(pin->ext, pin->pin, val);
    }
    pin->state = val;
}

bool GpioClass::read(GpioPin *pin)
{
    if (pin->ext == nullptr) {
        return (digitalRead(pin->pin) == HIGH) ? true : false;
    } else {
        return Extenders.read(pin->ext, pin->pin);
    }
    return false;
}

bool GpioClass::getState(GpioPin *pin)
{
    if (pin->mode == GPIO_MOD_INPUT) {
        return read(pin);
    } else {
        return pin->state;
    }
}

bool GpioClass::getPinById(uint16_t id, GpioPin **pin)
{
    for (uint16_t i = 0; i < _pins.size(); i++) {
        if (_pins[i].id == id && _pins[i].enabled) {
            *pin = &_pins[i];
            return true;
        }
    }
    return false;
}

void GpioClass::getPins(std::vector<GpioPin *> &pins)
{
    for (uint16_t i = 0; i < _pins.size(); i++) {
        if (_pins[i].enabled) {
            pins.push_back(&_pins[i]);
        }
    }
}

void GpioClass::getPinsByType(GpioType type, std::vector<GpioPin *> &pins)
{
    for (uint16_t i = 0; i < _pins.size(); i++) {
        if (_pins[i].enabled && _pins[i].type == type) {
            pins.push_back(&_pins[i]);
        }
    }
}

void GpioClass::setMode(GpioPin *pin, GpioMode mode, GpioPull pull)
{
    uint8_t m = INPUT;

    if (mode == GPIO_MOD_INPUT && pull == GPIO_PULL_NONE) {
        m = INPUT;
    } else if (mode == GPIO_MOD_INPUT && pull == GPIO_PULL_UP) {
        m = INPUT_PULLUP;
    } else if (mode == GPIO_MOD_INPUT && pull == GPIO_PULL_DOWN) {
        m = INPUT_PULLDOWN;
    } else if (mode == GPIO_MOD_OUTPUT && pull == GPIO_PULL_NONE) {
        m = OUTPUT;
    } else {
        Log.error(F("GPIO"), "GPIO id: " +String(pin->id)+ String(F(" failed to set mode. Unknown mode: ")) + String(mode) + " pull: " + String(pull));
    }
  
    if (pin->ext == nullptr) {
        return pinMode(pin->pin, m);
    } else {
        return Extenders.setPinMode(pin->ext, pin->pin, m);
    }
}

/*********************************************************************/
/*                                                                   */
/*                          PRIVATE FUNCTIONS                        */
/*                                                                   */
/*********************************************************************/

void GpioClass::_beginPin(GpioPin *pin)
{
    uint8_t val = INPUT;

    switch (pin->mode) {
        case GPIO_MOD_INPUT:
            if (pin->pull == GPIO_PULL_DOWN) {
                val = INPUT_PULLDOWN;
            } else if (pin->pull == GPIO_PULL_UP) {
                val = INPUT_PULLUP;
            } else {
                val = INPUT;
            }
            break;

        case GPIO_MOD_OUTPUT:
            val = OUTPUT;
            break;
    }

    if (pin->ext == nullptr) {
        pinMode(pin->pin, val);
    } else {
        Extenders.setPinMode(pin->ext, pin->pin, val);
    }
}

GpioClass Gpio;
