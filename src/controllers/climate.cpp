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

void ClimateCtrlClass::setEnabled(bool enabled)
{
    _enabled = enabled;
}

bool &ClimateCtrlClass::getEnabled()
{
    return _enabled;
}

void ClimateCtrlClass::getEnabledClimateZone(std::vector<ClimateZone*> &zones)
{
    for (size_t i = 0; i < _zones.size(); i++) {
        if (_zones[i].enabled) {
            zones.push_back(&_zones[i]);
        }
    }
}

void ClimateCtrlClass::begin()
{
    std::vector<ClimateZone *> zones;

    getEnabledClimateZone(zones);

    for (size_t i = 0; i < zones.size(); i++) {
        if (!zones[i]->enabled) {
            continue;
        }
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
    getEnabledClimateZone(zones);

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

void ClimateCtrlClass::setStatus(ClimateZone *zone, bool status, bool save)
{
    zone->status = status;

    Log.info(F("CLIMATE"), String(F("Climate zone ")) + zone->name + String(F(" changed status to ")) + (zone->status ? "ON" : "OFF"));

    if (zone->relay != nullptr) { Gpio.write(zone->relay, status); }

    if (save) {
        if (EeDb.getEnabled()) {
            EeDbClimate  db;
            if (EeDb.loadClimateDb(db)) {
                if (EeDb.setClimateStatus(db, zone->id, status)) {
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

    getEnabledClimateZone(zones);

    if (EeDb.getEnabled()) {
        EeDbClimate db;
        bool        status;

        if (EeDb.loadClimateDb(db)) {
            for (size_t i = 0; i < zones.size(); i++) {
                if (EeDb.getClimateStatus(db, zones[i]->id, status)) {
                    Log.info(F("CLIMATE"), String(F("Load climate status from EEPROM. Id: ")) + String(zones[i]->id));
                    setStatus(zones[i], status, false);
                } else {
                    Log.error(F("CLIMATE"), String(F("Failed to set climate status to EEPROM. Id: ")) + String(zones[i]->id));
                }
            }
        } else {
            Log.error(F("CLIMATE"), String(F("Failed to load climate DB from EEPROM.")));
        }
    }
    return true;
}

ClimateCtrlClass ClimateCtrl;
