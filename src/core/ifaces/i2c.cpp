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

#include "core/ifaces/i2c.hpp"
#include <Wire.h>

IfI2C::IfI2C(const String &name, uint8_t sda, uint8_t scl, bool extended)
{
    _name = name;
    _extended = extended;
    _pins[I2C_PIN_SDA] = sda;
    _pins[I2C_PIN_SCL] = scl;
}

void IfI2C::setPin(I2cPin pin, uint8_t gpio)
{
    _pins[pin] = gpio;
}

uint8_t IfI2C::getPin(I2cPin pin) const
{
    return _pins[pin];
}

const String &IfI2C::getName() const
{
    return _name;
}

void IfI2C::findDevices(std::vector<unsigned> &devices)
{
    Wire.begin(_pins[I2C_PIN_SDA], _pins[I2C_PIN_SCL]);
    for (auto addr = I2C_MIN_ADDR; addr < I2C_MAX_ADDR; addr++) {
        Wire.beginTransmission(addr);
        auto error = Wire.endTransmission();
        if (error == 0) {
            devices.push_back(addr);
        }
    }
}

bool IfI2C::getExtended() const
{
    return _extended;
}

void IfI2C::setExtended(bool state)
{
    _extended = state;
}

IfType IfI2C::getType() const
{
    return IF_TYPE_I2C;
}

void IfI2C::setName(const String &name)
{
    _name = name;
}
