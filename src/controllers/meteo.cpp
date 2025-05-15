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

#include "controllers/meteo.hpp"
#include "core/ifaces/ow.hpp"
#include "utils/log.hpp"

/*********************************************************************/
/*                                                                   */
/*                          PUBLIC FUNCTIONS                         */
/*                                                                   */
/*********************************************************************/

MeteoCtrlClass::MeteoCtrlClass()
{
    for (size_t i = 0; i < _sensors.size(); i++) {
        memset(&_sensors[i], 0x0, sizeof(MeteoSensor));
        _sensors[i].id = i + 1;
    }
}

void MeteoCtrlClass::setEnabled(bool enabled)
{
    _enabled = enabled;
}

bool& MeteoCtrlClass::getEnabled()
{
    return _enabled;
}

bool MeteoCtrlClass::setSensor(size_t index, MeteoSensor *sensor)
{
    if (index > (_sensors.size() - 1)) {
        return false;
    }

    memcpy(&_sensors[index], sensor, sizeof(MeteoSensor));

    return true;
}

void MeteoCtrlClass::findDsSensors(std::vector<uint64_t> &sensors)
{
    OneWireBus  *bus;

    if (OneWireIf.getOWBusById(PROF_OW_TEMP, &bus)) {
        OneWireIf.findDevices(bus, sensors);
    } else {
        Log.error(F("METEO"), F("I2C bus OneWireTemp not found"));
    }
}

bool MeteoCtrlClass::getSensor(size_t index, MeteoSensor **sens)
{
    for (size_t i = 0; i < _sensors.size(); i++) {
        if (index == i) {
            *sens = &_sensors[i];
            return true;
        }
    }
    return false;
}

void MeteoCtrlClass::begin()
{
    std::vector<MeteoSensor *>  sensors;
    OneWireBus                  *bus;

    getEnabledSensors(sensors);
    
    for (auto sensor : sensors) {
        switch (sensor->type) {
            case METEO_SENSOR_AM2302:
                sensor->dht.setup(sensor->pin->pin, DHTesp::AM2302);
                break;

            case METEO_SENSOR_DHT22:
                sensor->dht.setup(sensor->pin->pin, DHTesp::DHT22);
                break;
        }
    }

    if (OneWireIf.getOWBusById(PROF_OW_TEMP, &bus)) {
        _ds.setPin(bus->pin);
    } else {
        Log.error(F("METEO"), F("I2C bus OneWireTemp not found"));
    }
}

void MeteoCtrlClass::getEnabledSensors(std::vector<MeteoSensor *> &sensors)
{
    for (size_t i = 0; i < _sensors.size(); i++) {
        if (_sensors[i].enabled) {
            sensors.push_back(&_sensors[i]);
        }
    }
}

std::array<MeteoSensor, METEO_SENSOR_COUNT> MeteoCtrlClass::getSensors()
{
    return _sensors;
}

void MeteoCtrlClass::loop()
{
    if (!_enabled) return;

    std::vector<MeteoSensor *> sensors;
    getEnabledSensors(sensors);

    if (sensors.size() == 0) return;

    if (!_reqSend) {
        if ((millis() - _timerDs >= METEO_SENS_TIMER_DS_MS)) {
            _timerDs = millis();
            if (_ds.requestTemp()) {
                _reqSend = true;
            } else {
                Serial.println("FAIL REQUEST");
            }
        }
    }

    if (!_ready) {
        if ((millis() - _timer >= METEO_SENS_TIMER_MS)) {
            _ready = true;
            _timer = millis();
        }
    } else {
        _readData(sensors[_curSensor]);

        if (_curSensor < (sensors.size() - 1)) {
            _curSensor++;
        } else {
            _curSensor = 0;
            _ready = false;
            _timer = millis();
            _reqSend = false;
        }
    }
}

/*********************************************************************/
/*                                                                   */
/*                          PRIVATE FUNCTIONS                        */
/*                                                                   */
/*********************************************************************/

void MeteoCtrlClass::_readData(MeteoSensor *sensor)
{
    if (!sensor->enabled) return;

    if (sensor->type == METEO_SENSOR_AM2302) {
        auto data = sensor->dht.getTempAndHumidity();
        if (data.temperature != NAN && data.humidity != NAN) {
            if (sensor->error != 0) {
                Log.info(F("METEO"), String(F("Meteo sensor AM2302: ")) + sensor->name + " is online");
                sensor->error = 0;
            }
        } else {
            sensor->error++;
            if (sensor->error == METEO_SENSOR_ERROR_MAX) {
                Log.error(F("METEO"), String(F("Failed to read AM2302 sensor: ")) + sensor->name);
            }
        }
    } else if (sensor->type == METEO_SENSOR_DS18B20) {
        if (_ds.ready()) {
            if (_ds.readTemp(sensor->addr)) {
                if (sensor->error != 0) {
                    Log.info(F("METEO"), String(F("Meteo sensor DS18B20: ")) + sensor->name + " is online");
                    sensor->error = 0;
                }
                sensor->data.temp = _ds.getTemp();
            } else {
                sensor->error++;
                if (sensor->error == METEO_SENSOR_ERROR_MAX) {
                    Log.error(F("METEO"), String(F("Failed to read DS18B20 sensor: ")) + sensor->name);
                }
            }
        }
    }
}

MeteoCtrlClass MeteoCtrl;
