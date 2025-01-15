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

#include "core/clock.hpp"
#include "boards/boards.hpp"
#include "core/ifaces/i2c.hpp"
#include "utils/log.hpp"

void ClockClass::begin()
{
    I2cBus  *bus;

    if (!I2C.getI2cBusById(ActiveBoard.clock.i2c, &bus)) {
        Log.error(F("CLOCK"), F("I2C bus clock not found"));
    } else {
        _ds.begin(bus->wire, ActiveBoard.clock.addr);
        Log.info(F("CLOCK"), String(F("Clock inited at bus: ")) +
                    String(bus->id) + String(F(" addr: 0x")) +
                    String(ActiveBoard.clock.addr, HEX));
    }
}

float ClockClass::getTemp()
{
    return _ds.getTemp();
}

void ClockClass::getTime(Datime &time)
{
    time = _ds.getTime();
}

bool ClockClass::setTime(Datime &time)
{
    return _ds.setTime(time);
}

bool ClockClass::setTimeUTC(Datime &time)
{
    time.addHours(_utc);
    return _ds.setTime(time);
}

uint8_t &ClockClass::getUTC()
{
    return _utc;
}

void ClockClass::setUTC(uint8_t utc)
{
    _utc = utc;
}

ClockClass Clock;
