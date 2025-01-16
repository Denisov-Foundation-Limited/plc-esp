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

#ifndef __METEO_HPP__
#define __METEO_HPP__

#include <Arduino.h>
#include <vector>
#include <GyverDS18.h>
#include <DHTesp.h>

#include "core/ifaces/ow.hpp"

#define METEO_SENS_TIMER_MS     5000
#define METEO_SENSOR_ERROR_MAX  10

#define METEO_SENSOR_COUNT  64

typedef enum {
    METEO_SENSOR_BME280,
    METEO_SENSOR_DHT22,
    METEO_SENSOR_DS18B20,
    METEO_SENSOR_AM2302
} MeteoSensorType;

typedef struct {
    float temp;
    float hum;
    float pres;
} MeteoData;

typedef struct {
    size_t          id;
    String          name;
    MeteoSensorType type;
    MeteoData       data;
    unsigned        error;
    GpioPin         *pin;
    DHTesp          dht;
    bool            enabled;
} MeteoSensor;

class MeteoCtrlClass
{
public:
    MeteoCtrlClass();
    void setSensor(size_t index, MeteoSensor &sensor);
    void getEnabledSensors(std::vector<MeteoSensor *> &sensors);
    void begin();
    void loop();

private:
    bool                                        _ready = false;
    bool                                        _enabled = false;
    unsigned                                    _timer = 0;
    unsigned                                    _curSensor = 0;
    std::array<MeteoSensor, METEO_SENSOR_COUNT> _sensors;

    void _readData(MeteoSensor *sensor);
};

extern MeteoCtrlClass MeteoCtrl;

#endif /* __METEO_HPP__ */