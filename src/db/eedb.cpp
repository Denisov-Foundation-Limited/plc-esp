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
    I2cBus      *bus;
    size_t      size;
    uint16_t    magic;

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
                        String(ActiveBoard.plc.temp.addr, HEX) + " used: " + String(getOffset(EE_DB_OFFSET_MAX)) + " b of " + String(size) + " b");
            }
        } else {
            Log.error(F("EEDB"), String(F("EEPROM not found")));
            return false;
        }
    }

    _ee.readBlock(getOffset(EE_DB_OFFSET_MAGIC), (uint8_t *)&magic, sizeof(uint16_t));

    if (magic != EE_DB_MAGIC) {
        uint8_t data[EE_DB_FLASH_BLOCK];
        unsigned offset = 0;

        memset(data, 0x0, EE_DB_FLASH_BLOCK);

        Log.info(F("EEDB"), F("Magic not found. Erasing flash memory"));

        for (unsigned i = 0; i < EE_DB_BLOCK_COUNT; i++) {
            Log.info(F("EEDB"), String(F("Erasing block ... ")) + "0x" + String(offset, 16));
            _ee.writeBlock(offset, (uint8_t *)data, EE_DB_FLASH_BLOCK);
            offset += EE_DB_FLASH_BLOCK;
        }

        Log.info(F("EEDB"), F("Erasing complete"));

        magic = EE_DB_MAGIC;
        _ee.writeBlock(getOffset(EE_DB_OFFSET_MAGIC), (uint8_t *)&magic, sizeof(uint16_t));
    }

    return true;
}

bool &EepromDbClass::getEnabled()
{
    return _enabled;
}

size_t EepromDbClass::getOffset(EeDbOffset offset)
{
    size_t out = 0;

    switch (offset) {
        case EE_DB_OFFSET_MAGIC:
            return out;

        case EE_DB_OFFSET_SOCKET:
            return getOffset(EE_DB_OFFSET_MAGIC) + sizeof(uint16_t);

        case EE_DB_OFFSET_CLIMATE:
            return getOffset(EE_DB_OFFSET_SOCKET) + sizeof(EeDbSocket);

        case EE_DB_OFFSET_MAX:
            return getOffset(EE_DB_OFFSET_CLIMATE) + sizeof(EeDbClimate);
    }
    return out;
}

bool EepromDbClass::loadClimateDb(EeDbClimate &data)
{
    if (_ee.isConnected()) {
        _ee.readBlock(getOffset(EE_DB_OFFSET_CLIMATE), (uint8_t *)&data, sizeof(EeDbClimate));
        return true;
    }
    return false;
}

bool EepromDbClass::saveClimateDb(EeDbClimate &data)
{
    size_t offset = getOffset(EE_DB_OFFSET_CLIMATE);

    if (_ee.isConnected()) {
        _ee.setBlock(offset, 0x0, sizeof(EeDbClimate));
        _ee.writeBlock(offset, (uint8_t *)&data, sizeof(EeDbClimate));
        return true;
    }

    return false;
}

bool EepromDbClass::getClimateData(EeDbClimate &data, uint8_t id, bool &status, int8_t &temp, uint8_t &delta)
{
    uint8_t curId = id - 1;

    if (id > EE_DB_ID_MAX) {
        return false;
    }

    if (data.status & (1 << curId)) {
        status = true;
    } else {
        status = false;
    }

    temp = data.tempCfg[curId].temp;
    delta = data.tempCfg[curId].delta;

    return true;
}

bool EepromDbClass::setClimateData(EeDbClimate &data, uint8_t id, bool status, int8_t temp, uint8_t delta)
{
    uint8_t curId = id - 1;

    if (id > EE_DB_ID_MAX) {
        return false;
    }

    if (status)
        data.status |= (1 << curId);
    else
        data.status &= ~(1 << curId);

    data.tempCfg[curId].temp = temp;
    data.tempCfg[curId].delta = delta;

    return true;
}

bool EepromDbClass::loadSocketDb(EeDbSocket &data)
{
    if (_ee.isConnected()) {
        _ee.readBlock(getOffset(EE_DB_OFFSET_SOCKET), (uint8_t *)&data, sizeof(EeDbSocket));
        return true;
    }
    return false;
}

bool EepromDbClass::saveSocketDb(EeDbSocket &data)
{
    size_t offset = getOffset(EE_DB_OFFSET_SOCKET);

    if (_ee.isConnected()) {
        _ee.setBlock(offset, 0x0, sizeof(EeDbSocket));
        _ee.writeBlock(offset, (uint8_t *)&data, sizeof(EeDbSocket));
        return true;
    }

    return false;
}

bool EepromDbClass::getSocketStatus(EeDbSocket &data, uint8_t id, bool &status)
{
    uint8_t curId = id - 1;

    if (id > EE_DB_ID_MAX) {
        return false;
    }

    if (data.status & (1 << curId)) {
        status = true;
    } else {
        status = false;
    }

    return true;
}

bool EepromDbClass::setSocketStatus(EeDbSocket &data, uint8_t id, bool status)
{
    uint8_t curId = id - 1;

    if (id > EE_DB_ID_MAX) {
        return false;
    }

    if (status)
        data.status |= (1 << curId);
    else
        data.status &= ~(1 << curId);

    return true;
}

EepromDbClass EeDb;
