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

#include <controllers/tank.hpp>
#include <db/eedb.h>

/*********************************************************************/
/*                                                                   */
/*                          PUBLIC FUNCTIONS                         */
/*                                                                   */
/*********************************************************************/

TankCtrlClass::TankCtrlClass()
{
    for (size_t i = 0; i < _tanks.size(); i++) {
        memset(&_tanks[i], 0x0, sizeof(Tank));
        _tanks[i].id = i + 1;
        _tanks[i].level = 0xFF;
    }
}

void TankCtrlClass::setEnabled(bool enabled)
{
    _enabled = enabled;
}

bool TankCtrlClass::getEnabled() const
{
    return _enabled;
}

void TankCtrlClass::getTanks(bool enabled, std::vector<Tank *> &tanks)
{
    for (size_t i = 0; i < _tanks.size(); i++) {
        if (enabled) {
            if (_tanks[i].enabled) {
                tanks.push_back(&_tanks[i]);
            }
        } else {
            tanks.push_back(&_tanks[i]);
        }
    }
}

bool TankCtrlClass::setTank(size_t index, Tank *tank)
{
    if (index > (_tanks.size() - 1)) {
        return false;
    }

    memcpy(&_tanks[index], tank, sizeof(Tank));

    return true;
}

void TankCtrlClass::begin(bool load)
{
    std::vector<Tank *> tanks;

    getTanks(true, tanks);

    for (size_t i = 0; i < tanks.size(); i++) {
        if (tanks[i]->pump != nullptr) {
            Gpio.setMode(tanks[i]->pump, GPIO_MOD_OUTPUT, GPIO_PULL_NONE);
            Gpio.write(tanks[i]->pump, false);
        }

        if (tanks[i]->valve != nullptr) {
            Gpio.setMode(tanks[i]->valve, GPIO_MOD_OUTPUT, GPIO_PULL_NONE);
            Gpio.write(tanks[i]->valve, false);
        }

        for (uint8_t j = 0; j < TANK_LEVEL_MAX; j++) {
            if (tanks[i]->levels[j] != nullptr) {
                Gpio.setMode(tanks[i]->levels[j], GPIO_MOD_INPUT, GPIO_PULL_UP);
            }
        }
    }
    if (load) {
        _loadStates();
    }
}

void TankCtrlClass::loop()
{
    if (!_enabled) return;

    std::vector<Tank *> tanks;
    getTanks(true, tanks);

    if (tanks.size() == 0) return;

    if (!_reading) {
        if ((millis() - _timer) >= TANK_PROCESS_MS) {
            _reading = true;
            _timer = millis();
        }
    } else {
        _processTank(tanks[_curTank], false);

        if (_curTank < (tanks.size() - 1)) {
            _curTank++;
        } else {
            _curTank = 0;
            _reading = false;
            _timer = millis();
        }
    }
}

void TankCtrlClass::setStatus(Tank *tank, bool status, bool save)
{
    if (tank->status == status)
        return;

    tank->status = status;

    Log.info(F("TANK"), String(F("Tank ")) + tank->name + String(F(" changed status to ")) + (tank->status ? "ON" : "OFF"));

    if (save) {
        if (EeDb.getEnabled()) {
            EeDbTank  db;
            if (EeDb.loadTankDb(db)) {
                if (EeDb.setTankStatus(db, tank->id, status)) {
                    if (EeDb.saveTankDb(db)) {
                        Log.info(F("TANK"), String(F("Tank status saved to EEPROM. Id: ")) + String(tank->id));
                    } else {
                        Log.error(F("TANK"), String(F("Failed to save tank status to EEPROM. Id: ")) + String(tank->id));
                    }
                } else {
                    Log.error(F("TANK"), String(F("Failed to set tank status to EEPROM. Id: ")) + String(tank->id));
                }
            } else {
                Log.error(F("TANK"), String(F("Failed to load tank status from EEPROM. Id: ")) + String(tank->id));
            }
        }
    }

    _processTank(tank, true);

    if (!status) {
        if (tank->pump != nullptr) {
            Gpio.write(tank->pump, false);
        }
        if (tank->valve != nullptr) {
            Gpio.write(tank->valve, false);
        }
    }
}

/*********************************************************************/
/*                                                                   */
/*                         PRIVATE FUNCTIONS                         */
/*                                                                   */
/*********************************************************************/

void TankCtrlClass::_processTank(Tank *tank, bool force)
{
    uint8_t level = 0;

    for (uint8_t j = 0; j < TANK_LEVEL_MAX; j++) {
        if (tank->levels[j] != nullptr) {
            if (Gpio.read(tank->levels[j])) {
                level += 30;
            }
        }
    }

    if (level == 90)
        level = 100;

    if (tank->level != level || force) {
        tank->level = level;

        Log.info(F("TANK"), String(F("Tank ")) + tank->name + String(F(" water level changed to ")) + String(level) + "%");

        if (tank->status) {
            if (level < 100) {
                if (tank->valve != nullptr) {
                    Gpio.write(tank->valve, true);
                    Log.info(F("TANK"), String(F("Tank ")) + tank->name + String(F(" valve is openned")));
                }
            } else {
                if (tank->valve != nullptr) {
                    Gpio.write(tank->valve, false);
                    Log.info(F("TANK"), String(F("Tank ")) + tank->name + String(F(" valve is closed")));
                }
            }
            if (level > 0) {
                if (tank->pump != nullptr) {
                    Gpio.write(tank->pump, true);
                    Log.info(F("TANK"), String(F("Tank ")) + tank->name + String(F(" pump is on")));
                }
            } else {
                if (tank->pump != nullptr) {
                    Gpio.write(tank->pump, false);
                    Log.info(F("TANK"), String(F("Tank ")) + tank->name + String(F(" pump is off")));
                }
            }
        }
    }
}

bool TankCtrlClass::_loadStates()
{
    std::vector<Tank *> tanks;

    getTanks(true, tanks);

    if (EeDb.getEnabled()) {
        EeDbTank    db;
        bool        status;

        if (EeDb.loadTankDb(db)) {
            for (size_t i = 0; i < tanks.size(); i++) {
                if (EeDb.getTankStatus(db, tanks[i]->id, status)) {
                    Log.info(F("TANK"), String(F("Load socket tank from EEPROM. Id: ")) + String(tanks[i]->id));
                    setStatus(tanks[i], status, false);
                } else {
                    Log.error(F("TANK"), String(F("Failed to set tank status to EEPROM. Id: ")) + String(tanks[i]->id));
                }
            }
        } else {
            Log.error(F("TANK"), String(F("Failed to load tank DB from EEPROM.")));
        }
    }

    return true;
}

TankCtrlClass TankCtrl;
