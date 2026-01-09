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

#include "stack/stack.hpp"
#include "net/api/apiserver.hpp"

/*********************************************************************/
/*                                                                   */
/*                          PUBLIC FUNCTIONS                         */
/*                                                                   */
/*********************************************************************/

StackClass::StackClass()
{
    for (size_t i = 0; i < _units.size(); i++) {
        memset(&_units[i], 0x0, sizeof(StackUnit));
        _units[i].id = i + 1;
    }
}

bool StackClass::setUnit(size_t id, StackUnit *unit)
{
    if (id > (_units.size() - 1)) {
        return false;
    }

    memcpy(&_units[id], unit, sizeof(StackUnit));

    return true;
}

void StackClass::setEnabled(bool enabled)
{
    _enabled = enabled;
}

bool StackClass::getEnabled() const
{
    return _enabled;
}

void StackClass::begin()
{

}

void StackClass::loop()
{
    if (!_enabled) return;

    std::vector<StackUnit *> units;
    getUnits(true, units);

    if (units.size() == 0) return;

    if (!_reading) {
        if ((millis() - _timer) >= STACK_UNIT_PROC_MS) {
            _reading = true;
            _timer = millis();
        }
    } else {
        _processUnit(units[_curUnit]);

        if (_curUnit < (units.size() - 1)) {
            _curUnit++;
        } else {
            _curUnit = 0;
            _reading = false;
            _timer = millis();
        }
    }
}

void StackClass::getUnits(bool enabled, std::vector<StackUnit*> &units)
{
    for (size_t i = 0; i < _units.size(); i++) {
        if (enabled) {
            if (_units[i].enabled) {
                units.push_back(&_units[i]);
            }
        } else {
            units.push_back(&_units[i]);
        }
    }
}

bool StackClass::getUnit(uint8_t unitId, StackUnit** unit)
{
    for (size_t i = 0; i < _units.size(); i++) {
        if (_units[i].enabled && _units[i].id == unitId) {
            *unit = &_units[i];
            return true;
        }
    }
    return false;
}

/*********************************************************************/
/*                                                                   */
/*                          PRIVATE FUNCTIONS                        */
/*                                                                   */
/*********************************************************************/

void StackClass::_processUnit(StackUnit *unit)
{
    Serial.println(unit->ip);
    if (_client.begin(unit->ip, API_SERVER_PORT)) {
        _client.setTimeout(100);
        _client.setConnectTimeout(100);

        int code = _client.GET();
        Serial.println(String( code ));
        if (code == 200) {
            unit->online = true;
        } else {
            unit->online = false;
        }
    }
    _client.end();
}

StackClass Stack;
