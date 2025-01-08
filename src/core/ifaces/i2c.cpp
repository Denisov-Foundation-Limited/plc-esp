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

#include "core/ifaces/i2c.hpp"
#include <Wire.h>
#include "boards/boards.hpp"
#include "utils/log.hpp"

bool I2cClass::begin()
{
    for (uint8_t i = 0; i < PROF_I2C_MAX; i++) {
        auto bus = ActiveBoard.interfaces.i2c[i];

        _i2c[i].enabled = true;
        _i2c[i].sda = bus.sda;
        _i2c[i].scl = bus.scl;
        _i2c[i].id = bus.id;
        
        if (i == 0) {
            if (!Wire.begin(_i2c[i].sda, _i2c[i].scl, I2C_DEFAULT_SPEED)) {
                Log.error(F("I2C"), "I2C id: " +String(bus.id)+ " init failed.");
                return false;
            }
            _i2c[i].wire = &Wire;
        } else {
            if (!Wire1.begin(_i2c[i].sda, _i2c[i].scl, I2C_DEFAULT_SPEED)) {
                Log.error(F("I2C"), "I2C id: " +String(bus.id)+ " init failed.");
                return false;
            }
            _i2c[i].wire = &Wire1;
        }
        Log.info(F("I2C"), "I2C id: " + String(_i2c[i].id) + " init at sda: " + String(_i2c[i].sda) + " scl: " + String(_i2c[i].scl));
    }
    return true;
}

bool I2cClass::getI2cBusById(uint8_t id, I2cBus **bus)
{
    for (uint8_t i = 0; i < _i2c.size(); i++) {
        if (_i2c[i].id == id && _i2c[i].enabled) {
            *bus = &_i2c[i];
            return true;
        }
    }
    return false;
}

void I2cClass::getI2cBuses(std::vector<I2cBus *> &buses)
{
    for (uint8_t i = 0; i < _i2c.size(); i++) {
        if (_i2c[i].enabled) {
            buses.push_back(&_i2c[i]);
        }
    }
}

void I2cClass::findDevices(I2cBus *bus, std::vector<byte> &devs)
{
    byte    address, error;

    for (address = I2C_SCAN_ADDR_FIRST; address < I2C_SCAN_ADDR_LAST; address++) {
        bus->wire->beginTransmission(address);
        error = bus->wire->endTransmission();
        if (error == 0) {
            devs.push_back(address);
        }
    }
}

I2cClass I2C;
