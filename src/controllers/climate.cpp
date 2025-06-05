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

#include <controllers/climate.hpp>
#include "db/eedb.h"

/*********************************************************************/
/*                                                                   */
/*                          PUBLIC FUNCTIONS                         */
/*                                                                   */
/*********************************************************************/

ClimateCtrlClass::ClimateCtrlClass()
{
    for (size_t i = 0; i < _zones.size(); i++) {
        memset(&_zones[i], 0x0, sizeof(ClimateZone));
        _zones[i].id = i + 1;
        _zones[i].delta = 1;
        _zones[i].temp = 24;
    }
}

bool ClimateCtrlClass::setZone(size_t id, ClimateZone *zone)
{
    if (id > (_zones.size() - 1)) {
        return false;
    }

    memcpy(&_zones[id], zone, sizeof(ClimateZone));

    return true;
}

void ClimateCtrlClass::setEnabled(bool enabled)
{
    _enabled = enabled;
}

bool &ClimateCtrlClass::getEnabled()
{
    return _enabled;
}

void ClimateCtrlClass::getZones(bool enabled, std::vector<ClimateZone*> &zones)
{
    for (size_t i = 0; i < _zones.size(); i++) {
        if (enabled) {
            if (_zones[i].enabled) {
                zones.push_back(&_zones[i]);
            }
        }
        else {
            zones.push_back(&_zones[i]);
        }
    }
}

void ClimateCtrlClass::begin()
{
    std::vector<ClimateZone *> zones;

    getZones(true, zones);

    for (size_t i = 0; i < zones.size(); i++) {
        if (zones[i]->relay != nullptr) {
            Gpio.setMode(zones[i]->relay, GPIO_MOD_OUTPUT, GPIO_PULL_NONE);
        }
        if (zones[i]->button != nullptr) {
            Gpio.setMode(zones[i]->button, GPIO_MOD_INPUT, GPIO_PULL_UP);
        }
    }
    _loadStates();
}

void ClimateCtrlClass::loop()
{
    if (!_enabled) return;

    std::vector<ClimateZone *> zones;
    getZones(true, zones);

    if (zones.size() == 0) return;

    if (!_reading) {
        if ((millis() - _tmrButton) >= CLIMATE_BUTTON_READ_MS) {
            _reading = true;
            _tmrButton = millis();
        }
    } else {
        _readButton(&_zones[_curZone]);

        if (_curZone < (zones.size() - 1)) {
            _curZone++;
        } else {
            _curZone = 0;
            _reading = false;
            _tmrButton = millis();
        }
    }

    if ((millis() - _tmrTemp) >= CLIMATE_TEMP_PROC_MS) {
        _tmrTemp = millis();

        _processTemp(&_zones[_curZone2]);

        if (_curZone2 < (zones.size() - 1)) {
            _curZone2++;
        } else {
            _curZone2 = 0;
            _tmrTemp = millis();
        }
    }
}

void ClimateCtrlClass::setTemp(ClimateZone *zone, int temp, bool save)
{
    zone->temp = temp;

    Log.info(F("CLIMATE"), String(F("Climate zone ")) + zone->name + String(F(" changed temperature to ")) + String(temp));

    if (save) {
        if (EeDb.getEnabled()) {
            EeDbClimate  db;
            if (EeDb.loadClimateDb(db)) {
                if (EeDb.setClimateData(db, zone->id, zone->status, zone->temp, zone->delta)) {
                    if (EeDb.saveClimateDb(db)) {
                        Log.info(F("CLIMATE"), String(F("Climate temp saved to EEPROM. Id: ")) + String(zone->id));
                    } else {
                        Log.error(F("CLIMATE"), String(F("Failed to save climate temp to EEPROM. Id: ")) + String(zone->id));
                    }
                } else {
                    Log.error(F("CLIMATE"), String(F("Failed to set climate temp to EEPROM. Id: ")) + String(zone->id));
                }
            } else {
                Log.error(F("CLIMATE"), String(F("Failed to load climate temp from EEPROM. Id: ")) + String(zone->id));
            }
        }
    }
}

int ClimateCtrlClass::getTemp(ClimateZone *zone)
{
    return zone->temp;
}

void ClimateCtrlClass::setDelta(ClimateZone *zone, unsigned delta, bool save)
{
    zone->delta = delta;

    Log.info(F("CLIMATE"), String(F("Climate zone ")) + zone->name + String(F(" changed delta to ")) + String(delta));

    if (save) {
        if (EeDb.getEnabled()) {
            EeDbClimate  db;
            if (EeDb.loadClimateDb(db)) {
                if (EeDb.setClimateData(db, zone->id, zone->status, zone->temp, zone->delta)) {
                    if (EeDb.saveClimateDb(db)) {
                        Log.info(F("CLIMATE"), String(F("Climate delta saved to EEPROM. Id: ")) + String(zone->id));
                    } else {
                        Log.error(F("CLIMATE"), String(F("Failed to save climate delta to EEPROM. Id: ")) + String(zone->id));
                    }
                } else {
                    Log.error(F("CLIMATE"), String(F("Failed to set climate delta to EEPROM. Id: ")) + String(zone->id));
                }
            } else {
                Log.error(F("CLIMATE"), String(F("Failed to load climate delta from EEPROM. Id: ")) + String(zone->id));
            }
        }
    }
}

unsigned ClimateCtrlClass::getDelta(ClimateZone *zone)
{
    return zone->delta;
}

void ClimateCtrlClass::setStatus(ClimateZone *zone, bool status, bool save)
{
    zone->status = status;

    Log.info(F("CLIMATE"), String(F("Climate zone ")) + zone->name + String(F(" changed status to ")) + (zone->status ? "ON" : "OFF"));

    if (zone->relay != nullptr) { Gpio.write(zone->relay, status); }

    if (save) {
        if (EeDb.getEnabled()) {
            EeDbClimate  db;
            if (EeDb.loadClimateDb(db)) {
                if (EeDb.setClimateData(db, zone->id, status, zone->temp, zone->delta)) {
                    if (EeDb.saveClimateDb(db)) {
                        Log.info(F("CLIMATE"), String(F("Climate status saved to EEPROM. Id: ")) + String(zone->id));
                    } else {
                        Log.error(F("CLIMATE"), String(F("Failed to save climate status to EEPROM. Id: ")) + String(zone->id));
                    }
                } else {
                    Log.error(F("CLIMATE"), String(F("Failed to set climate status to EEPROM. Id: ")) + String(zone->id));
                }
            } else {
                Log.error(F("CLIMATE"), String(F("Failed to load climate status from EEPROM. Id: ")) + String(zone->id));
            }
        }
    }
}

bool &ClimateCtrlClass::getStatus(ClimateZone *zone)
{
    return zone->status;
}

/*********************************************************************/
/*                                                                   */
/*                         PRIVATE FUNCTIONS                         */
/*                                                                   */
/*********************************************************************/

void ClimateCtrlClass::_readButton(ClimateZone *zone)
{
    if (zone->button == nullptr) return;

    if (!zone->reading) {
        if (Gpio.read(zone->button)) {
            Log.info(F("CLIMATE"), String(F("Climate zone ")) + zone->name + String(F(" button pressed")));
            setStatus(zone, !getStatus(zone), true);
            zone->reading = true;
            zone->timer = millis();
        }
    } else {
        if ((millis() - zone->timer) >= CLIMATE_BUTTON_WAIT_MS) {
            zone->reading = false;
        }
    }
}

void ClimateCtrlClass::_processTemp(ClimateZone *zone)
{
    if (!getStatus(zone)) return;
    if (zone == nullptr) return;
    if (zone->sensor == nullptr) return;
    if (zone->relay == nullptr) return;

    if (zone->type == CLIMATE_TYPE_HEAT) {
        if (zone->sensor->data.temp < zone->temp && !zone->work) {
            zone->work = true;
            Gpio.write(zone->relay, true);
        }
        if ((zone->sensor->data.temp >= (zone->temp + zone->delta)) && zone->work) {
            zone->work = false;
            Gpio.write(zone->relay, false);
        }
    } else if (zone->type == CLIMATE_TYPE_COOL) {
        if (zone->sensor->data.temp > zone->temp && !zone->work) {
            zone->work = true;
            Gpio.write(zone->relay, true);
        }
        if ((zone->sensor->data.temp <= (zone->temp + zone->delta)) && zone->work) {
            zone->work = false;
            Gpio.write(zone->relay, false);
        }
    }
}

bool ClimateCtrlClass::_loadStates()
{
    std::vector<ClimateZone *> zones;

    getZones(true, zones);

    if (EeDb.getEnabled()) {
        EeDbClimate db;
        bool        status;
        int8_t      temp;
        uint8_t     delta;

        if (EeDb.loadClimateDb(db)) {
            for (size_t i = 0; i < zones.size(); i++) {
                if (EeDb.getClimateData(db, zones[i]->id, status, temp, delta)) {
                    Log.info(F("CLIMATE"), String(F("Load climate data from EEPROM. Id: ")) + String(zones[i]->id));
                    setStatus(zones[i], status, false);
                    setTemp(zones[i], temp, false);
                    setDelta(zones[i], delta, false);
                } else {
                    Log.error(F("CLIMATE"), String(F("Failed to set climate data to EEPROM. Id: ")) + String(zones[i]->id));
                }
            }
        } else {
            Log.error(F("CLIMATE"), String(F("Failed to load climate DB from EEPROM.")));
        }
    }
    return true;
}

ClimateCtrlClass ClimateCtrl;
