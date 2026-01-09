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

#ifndef __RPC_METEO_HPP__
#define __RPC_METEO_HPP__

#include <Arduino.h>
#include <vector>

typedef enum {
    RPC_METEO_SENSOR_AM2302,
    RPC_METEO_SENSOR_DS18B20,
    RPC_METEO_SENSOR_BME280,
    RPC_METEO_SENSOR_DHT22    
} RpcMeteoSensorType;

typedef struct {
    RpcMeteoSensorType  type;
    int                 temp;
    int                 hum;
    int                 pres;
} RpcMeteoSensor;

class RpcMeteo
{
public:
    bool getMeteoSensors(std::vector<RpcMeteoSensor *> &sensors) const;

private:

};

#endif /* __RPC_METEO_HPP__ */