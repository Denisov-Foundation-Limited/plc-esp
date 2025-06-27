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
#include <core/ifaces/ow.hpp>
#include <core/plc.hpp>

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

void SecurityCtrlClass::getSensors(bool enabled, std::vector<SecuritySensor *> &sens)
{
    for (size_t i = 0; i < _sensors.size(); i++) {
        if (enabled) {
            if (_sensors[i].enabled) {
                sens.push_back(&_sensors[i]);
            }
        } else {
            sens.push_back(&_sensors[i]);
        }
    }
}

void SecurityCtrlClass::getKeys(bool enabled, std::vector<SecurityKey *> &sens)
{
    for (size_t i = 0; i < _keys.size(); i++) {
        if (enabled) {
            if (_keys[i].enabled) {
                sens.push_back(&_keys[i]);
            }
        } else {
            sens.push_back(&_keys[i]);
        }
    }
}

void SecurityCtrlClass::readKeysFromBus(std::vector<uint64_t> &serials)
{
    OneWireBus  *bus;

    if (OneWireIf.getOWBusById(PROF_OW_SECURITY, &bus)) {
        OneWireIf.findDevices(bus, serials);
    } else {
        Log.error(F("SECURITY"), F("I2C bus OneWireSecurity not found"));
    }
}

void SecurityCtrlClass::setStatus(bool status, bool save)
{
    if (_status == status)
        return;

    _status = status;

    Log.info(F("SECURITY"), String(F("Security status changed to ")) + ((status) ? "ON" : "OFF"));

    if (!status) {
        setAlarm(false);
        _curSensor = 0;
    }
}

bool SecurityCtrlClass::getStatus() const
{
    return _status;
}

bool SecurityCtrlClass::getAlarm() const
{
    return _alarm;
}

void SecurityCtrlClass::setEnabled(bool enabled)
{
    _enabled = enabled;
}

bool SecurityCtrlClass::getEnabled() const
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

void SecurityCtrlClass::begin(bool load)
{
    std::vector<SecuritySensor *> sensors;

    getSensors(true, sensors);

    for (size_t i = 0; i < sensors.size(); i++) {
        if (!sensors[i]->enabled) {
            continue;
        }
        if (sensors[i]->pin != nullptr) {
            Gpio.setMode(sensors[i]->pin, GPIO_MOD_INPUT, GPIO_PULL_UP);
        }
    }
    if (load) {
        _loadStates();
    }
}

void SecurityCtrlClass::loop()
{
    if (!_enabled) return;
    if (!_status) return;

    std::vector<SecuritySensor *> sensors;
    getSensors(true, sensors);

    if (sensors.size() == 0) return;

    if (!_reading) {
        if ((millis() - _timer) >= SECURITY_SENSOR_READ_MS) {
            _reading = true;
            _timer = millis();
        }
    } else {
        _readSensor(sensors[_curSensor]);

        if (_curSensor < (sensors.size() - 1)) {
            _curSensor++;
        } else {
            _curSensor = 0;
            _reading = false;
            _timer = millis();
        }
    }

    if (!_waitKey) {
        if ((millis() - _tmrKey) >= SECURITY_KEY_READ_MS) {
            _detectKeys();
            _tmrKey = millis();
        }
    } else {
        if ((millis() - _tmrKeyWait) >= SECURITY_KEY_WAIT_MS) {
            _waitKey = false;
            _tmrKeyWait = millis();
        }
    }
}

void SecurityCtrlClass::setAlarm(bool alarm)
{
    _alarm = alarm;

    if (!alarm) {
        std::vector<SecuritySensor *> sensors;
        getSensors(true, sensors);
        
        for (size_t i = 0; i < sensors.size(); i++) {
            sensors[i]->detected = false;
        }

        if (_relay != nullptr) { Gpio.write(_relay, false); }
        Plc.setBuzzer(PLC_MOD_SECURITY, false);
        Plc.setAlarm(PLC_MOD_SECURITY, false);
    } else {
        if (_relay != nullptr) { Gpio.write(_relay, true); }
        Plc.setBuzzer(PLC_MOD_SECURITY, true);
        Plc.setAlarm(PLC_MOD_SECURITY, true);
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

    getSensors(true, sensors);

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

bool SecurityCtrlClass::_checkKey(uint64_t serial, SecurityKey **key)
{
    for (size_t i = 0; i < _keys.size(); i++) {
        if (_keys[i].serial == serial) {
            *key = &_keys[i];
            return true;
        }
    }
    return false;
}

void SecurityCtrlClass::_detectKeys()
{
    OneWireBus              *bus;
    std::vector<uint64_t>   serials;
    SecurityKey             **foundKey;

    if (OneWireIf.getOWBusById(PROF_OW_SECURITY, &bus)) {
        OneWireIf.findDevices(bus, serials);
    } else {
        Log.error(F("SECURITY"), F("I2C bus OneWireSecurity not found"));
    }

    if (serials.size() != 0) {
        for (auto s : serials) {
            if (_checkKey(s, foundKey)) {
                _waitKey = true;
                Log.info(F("SECURITY"), String(F("Detected iButton key ")) + (*foundKey)->name);
                setStatus(!getStatus(), true);
            }
        }
    }
}

SecurityCtrlClass SecurityCtrl;
