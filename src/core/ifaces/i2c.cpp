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

I2CIface::I2CIface(const String &name, uint8_t sda, uint8_t scl)
{
    _name = name;
    _pins[I2C_PIN_SDA] = sda;
    _pins[I2C_PIN_SCL] = scl;
}

void I2CIface::setPin(I2cPin pin, uint8_t gpio)
{
    _pins[pin] = gpio;
}

uint8_t I2CIface::getPin(I2cPin pin) const
{
    return _pins[pin];
}

const String &I2CIface::getName() const
{
    return _name;
}

void I2CIface::findDevices(std::vector<unsigned> &devices)
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

IntType I2CIface::getType() const
{
    return INT_TYPE_I2C;
}

void I2CIface::setName(const String &name)
{
    _name = name;
}
