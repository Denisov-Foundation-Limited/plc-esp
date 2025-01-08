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

#ifndef __METEO_SENSOR_HPP__
#define __METEO_SENSOR_HPP__

typedef enum {
    METEO_SENSOR_DS18,
    METEO_SENSOR_DHT22,
    METEO_SENSOR_BME280
} MeteoSensorType;

class MeteoSensor
{
public:
    virtual void readData() = 0;
    virtual float getTemperature() = 0;
    virtual float getHumidity() = 0;
    virtual float getPressure() = 0;
    virtual const String &getName() = 0;
    virtual void setName(const String &name) = 0;
    virtual MeteoSensorType getType() = 0;
    virtual bool setEnabled(bool status) = 0;
    virtual bool getEnabled() = 0;
};

#endif /* __METEO_SENSOR_HPP__ */
