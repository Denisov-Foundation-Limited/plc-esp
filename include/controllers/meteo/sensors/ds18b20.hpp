/**********************************************************************/
/*                                                                    */
/* Programmable Logic Controller for ESP microcontrollers             */
/*                                                                    */
/* Copyright (C) 2024 Denisov Foundation Limited                      */
/* License: GPLv3                                                     */
/* Written by Sergey Denisov aka LittleBuster                         */
/* Email: DenisovFoundationLtd@gmail.com                              */
/*                                                                    */
/**********************************************************************/

#ifndef __DS18B20_HPP__
#define __DS18B20_HPP__

#include "controllers/meteo/sensors/msensor.hpp"
#include "core/ifaces/ow.hpp"
#include "utils/log.hpp"

#define DS18B20_ERRORS_MAX  4
#define DS18B20_ERROR_VALUE -127

class Ds18b20 : public MeteoSensor
{
public:
    Ds18b20(const String &name);
    Ds18b20(const String &name, const String &id);
    void setDSBus(GyverDS18 *ds);
    void setId(const String &id);
    String getId();
    void readData();
    float getTemperature();
    float getHumidity();
    float getPressure();
    MeteoSensorType getType();
    const String &getName();
    void setName(const String &name);
    bool setEnabled(bool status);
    bool getEnabled();

private:
    float       _temp = 0;
    float       _hum = 0;
    float       _pres = 0;
    unsigned    _error = 0;
    bool        _errorNotify = false;

    String      _name;
    uint64_t    _id = 0;
    bool        _enabled = false;

    GyverDS18   *_ds;
};

#endif /* __DS18B20_HPP__ */
