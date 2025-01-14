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

#include "db/eedb.h"
#include "boards/boards.hpp"
#include "core/ifaces/i2c.hpp"
#include "utils/log.hpp"

EepromDbClass::EepromDbClass()
{
}

bool EepromDbClass::begin()
{
    I2cBus *bus;
    size_t size;

    if (!I2C.getI2cBusById(ActiveBoard.eeprom.i2c, &bus)) {
        Log.error(F("EEDB"), F("I2C bus EEPROM not found"));
        return false;
    } else {
        _ee.begin(ActiveBoard.eeprom.addr, bus->wire, I2C_DEVICESIZE_24LC512, -1);
        if (_ee.isConnected()) {
            size = _ee.getDeviceSize();
            if (size != 0) {
                Log.info(F("EEDB"), String(F("EEPROM inited at bus: ")) +
                        String(bus->id) + String(F(" addr: 0x")) +
                        String(ActiveBoard.plc.temp.addr, HEX) + " size: " + String(size) + "b");
            }
        } else {
            Log.error(F("EEDB"), String(F("EEPROM not found")));
            return false;
        }
    }
    return true;
}

bool EepromDbClass::getEnabled() const
{
    return _enabled;
}

size_t EepromDbClass::getOffset(EeDbOffset offset)
{
    size_t out = 0;

    switch (offset) {
        case EE_DB_OFFSET_SOCKET:
            return out;
    }
    return out;
}

bool EepromDbClass::loadSocketDb(EeDbSocket &sockdb)
{
    if (_ee.isConnected()) {
        _ee.readBlock(getOffset(EE_DB_OFFSET_SOCKET), (uint8_t *)&sockdb, sizeof(EeDbSocket));
        return true;
    }
    return false;
}

bool EepromDbClass::saveSocketDb(EeDbSocket &sockdb)
{
    size_t offset = getOffset(EE_DB_OFFSET_SOCKET);

    if (_ee.isConnected()) {
        _ee.setBlock(offset, 0x0, sizeof(EeDbSocket));
        _ee.writeBlock(offset, (uint8_t *)&sockdb, sizeof(EeDbSocket));
        return true;
    }

    return false;
}

bool EepromDbClass::getSocketStatus(EeDbSocket &sockdb, uint8_t id, bool &status)
{
    if (id > RR_DB_ID_MAX) {
        return false;
    }

    if (sockdb.status & (1 << id)) {
        status = true;
    } else {
        status = false;
    }

    return true;
}

bool EepromDbClass::setSocketStatus(EeDbSocket &sockdb, uint8_t id, bool status)
{
    if (id > RR_DB_ID_MAX) {
        return false;
    }

    if (status)
        sockdb.status |= (1 << id);
    else
        sockdb.status &= ~(1 << id);

    return true;
}

EepromDbClass EeDb;
