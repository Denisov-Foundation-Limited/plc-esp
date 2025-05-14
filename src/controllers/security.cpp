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

#include <controllers/security.hpp>
#include <db/eedb.h>

/*********************************************************************/
/*                                                                   */
/*                          PUBLIC FUNCTIONS                         */
/*                                                                   */
/*********************************************************************/

GpioPin **SecurityCtrlClass::getRelay()
{
    return &_relay;
}

void SecurityCtrlClass::setRelay(GpioPin *relay)
{
    _relay = relay;
}

SecurityCtrlClass::SecurityCtrlClass()
{
    for (size_t i = 0; i < _sensors.size(); i++) {
        memset(&_sensors[i], 0x0, sizeof(SecuritySensor));
        _sensors[i].id = i + 1;
    }
    for (size_t i = 0; i < _keys.size(); i++) {
        memset(&_keys[i], 0x0, sizeof(SecurityKey));
        _keys[i].id = i + 1;
    }
}

void SecurityCtrlClass::getEnabledSensors(std::vector<SecuritySensor *> &sens)
{
    for (size_t i = 0; i < _sensors.size(); i++) {
        if (_sensors[i].enabled) {
            sens.push_back(&_sensors[i]);
        }
    }
}

void SecurityCtrlClass::getEnabledKeys(std::vector<SecurityKey *> &keys)
{
    for (size_t i = 0; i < _keys.size(); i++) {
        if (_keys[i].enabled) {
            keys.push_back(&_keys[i]);
        }
    }
}

void SecurityCtrlClass::setStatus(bool status, bool save)
{
    _status = status;

    if (!status) {
        setAlarm(false);
        _curSensor = 0;
    }
}

bool &SecurityCtrlClass::getStatus()
{
    return _status;
}

void SecurityCtrlClass::setEnabled(bool enabled)
{
    _enabled = enabled;
}

bool &SecurityCtrlClass::getEnabled()
{
    return _enabled;
}

bool SecurityCtrlClass::setSensor(size_t index, SecuritySensor *sensor)
{
    if (index > (_sensors.size() - 1)) {
        return false;
    }

    memcpy(&_sensors[index], sensor, sizeof(SecuritySensor));

    return true;
}

bool SecurityCtrlClass::setKey(size_t index, SecurityKey *key)
{
    if (index > (_keys.size() - 1)) {
        return false;
    }

    memcpy(&_keys[index], key, sizeof(SecurityKey));

    return true;
}

void SecurityCtrlClass::begin()
{
    std::vector<SecuritySensor *> sensors;

    getEnabledSensors(sensors);

    for (size_t i = 0; i < sensors.size(); i++) {
        if (!sensors[i]->enabled) {
            continue;
        }
        if (sensors[i]->pin != nullptr) {
            Gpio.setMode(sensors[i]->pin, GPIO_MOD_INPUT, GPIO_PULL_UP);
        }
    }
    _loadStates();
}

void SecurityCtrlClass::loop()
{
    if (!_enabled) return;
    if (!_status) return;

    std::vector<SecuritySensor *> sensors;
    getEnabledSensors(sensors);

    if (sensors.size() == 0) return;

    if (!_reading) {
        if ((millis() - _timer) >= SENSOR_READ_MS) {
            _reading = true;
            _timer = millis();
        }
    } else {
        _readSensor(&_sensors[_curSensor]);

        if (_curSensor < (sensors.size() - 1)) {
            _curSensor++;
        } else {
            _curSensor = 0;
            _reading = false;
            _timer = millis();
        }
    }
}

void SecurityCtrlClass::setAlarm(bool alarm)
{
    _alarm = alarm;

    if (!alarm) {
        std::vector<SecuritySensor *> sensors;
        getEnabledSensors(sensors);
        
        for (size_t i = 0; i < sensors.size(); i++) {
            sensors[i]->detected = false;
        }

        if (_relay != nullptr) { Gpio.write(_relay, false); }
        // unset buzzer
        // unset alarm led
    } else {
        if (_relay != nullptr) { Gpio.write(_relay, true); }
        // set buzzer
        // set alarm led
    }
}

/*********************************************************************/
/*                                                                   */
/*                          PRIVATE FUNCTIONS                        */
/*                                                                   */
/*********************************************************************/

void SecurityCtrlClass::_readSensor(SecuritySensor *sensor)
{
    if (sensor->pin != nullptr) {
        if (!sensor->reading) {
            _processSensor(sensor);
            sensor->reading = true;
            sensor->timer = millis();
        } else {
            if ((millis() - sensor->timer) >= SECURITY_SENSOR_WAIT_MS) {
                sensor->reading = false;
            }
        }
    }
}

void SecurityCtrlClass::_processSensor(SecuritySensor *sensor)
{
    bool detected = false;

    if (sensor->pin == nullptr) return;

    switch (sensor->type)
    {
        case SECURITY_SENSOR_REED:
            if (!Gpio.read(sensor->pin)) {
                detected = true;
            }
            break;

        case SECURITY_SENSOR_PIR:
            if (Gpio.read(sensor->pin)) {
                detected = true;
            }
            break;
    }

    if (detected && !sensor->detected) {
        sensor->detected = true;
        setAlarm(true);

        switch (sensor->type)
        {
            case SECURITY_SENSOR_REED:
                Log.warning(F("SECURITY"), String(F("Sensor ")) + sensor->name + String(F(" reed was openned")));
                break;

            case SECURITY_SENSOR_PIR:
                Log.warning(F("SECURITY"), String(F("Sensor ")) + sensor->name + String(F(" motion detected")));
                break;
        }
    }
}

bool SecurityCtrlClass::_loadStates()
{
    std::vector<SecuritySensor *> sensors;

    getEnabledSensors(sensors);

    if (EeDb.getEnabled()) {
        EeDbSecurity    db;
        bool            status;

        if (EeDb.loadSecurityDb(db)) {
            for (size_t i = 0; i < sensors.size(); i++) {
                if (EeDb.getSecurityStatus(db, status)) {
                    Log.info(F("SECURITY"), String(F("Load security status from EEPROM.")));
                    setStatus(status, false);
                } else {
                    Log.error(F("SECURITY"), String(F("Failed to set security status to EEPROM.")));
                }
            }
        } else {
            Log.error(F("SECURITY"), String(F("Failed to load security DB from EEPROM.")));
        }
    }
    return true;
}

SecurityCtrlClass SecurityCtrl;
