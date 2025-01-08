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

#include "controllers/meteo/meteo.hpp"
#include "controllers/meteo/sensors/ds18b20.hpp"

/*********************************************************************/
/*                                                                   */
/*                          PUBLIC FUNCTIONS                         */
/*                                                                   */
/*********************************************************************/

Ds18b20::Ds18b20(const String &name)
{
    _name = name;
}

Ds18b20::Ds18b20(const String &name, const String &id)
{
    _name = name;
    _id = strtoull(id.c_str(), NULL, 16);
}

const String &Ds18b20::getName()
{
    return _name;
}

void Ds18b20::setName(const String &name)
{
    _name = name;
}

void Ds18b20::setDSBus(GyverDS18 *ds)
{
    _ds = ds;
}

void Ds18b20::setId(const String &id)
{
    _id = strtoull(id.c_str(), NULL, 16);
}

String Ds18b20::getId()
{
    String sId(_id, 16);
    sId.toUpperCase();
    return sId;
}

void Ds18b20::readData()
{
    if (!_enabled || _ds == nullptr) return;

    if (_ds->ready()) {
        if (_ds->readTemp(_id)) {
            _error = 0;
            _errorNotify = false;
            _temp = _ds->getTemp();
        } else {
            if (_error == DS18B20_ERRORS_MAX) {
                if (!_errorNotify) {
                    _temp = DS18B20_ERROR_VALUE;
                    Log.error(F("METEO"), String(F("Failed to read ds18b20 sensor: ")) + _id);
                    _errorNotify = true;
                }
            } else {
                _error++;
            }
        }
    }
}

float Ds18b20::getTemperature()
{
    return _temp;
}

float Ds18b20::getHumidity()
{
    return _hum;
}

float Ds18b20::getPressure()
{
    return _pres;
}

MeteoSensorType Ds18b20::getType()
{
    return METEO_SENSOR_DS18;
}

bool Ds18b20::setEnabled(bool status)
{
    if (status && _ds == nullptr) {
        return false;
    }
    if (!status) {
        _temp = 0;
        _hum = 0;
        _pres = 0;
    }
    _enabled = status;
    return true;
}

bool Ds18b20::getEnabled()
{
    return _enabled;
}
