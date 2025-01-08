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

#include "sensors/msensor.hpp"
#include "core/ifaces/ow.hpp"
#include "controllers/ctrl.hpp"

#define METEO_SENS_TIMER_MS 5000
#define METEO_DS_TIMER_MS   3000

class MeteoCtrl : public Controller
{
public:
    MeteoCtrl(const String &name);
    CtrlType getType() const;
    void setEnabled(bool status);
    bool getEnabled() const;
    const String &getName() const;
    void setName(const String &name);
    void setOneWire(OneWireClass *ow);
    OneWireClass *getOneWire();
    void addSensor(MeteoSensor *sensor);
    const std::vector<MeteoSensor *> &getSensors();
    MeteoSensor *getSensor(const String &name);
    void begin();
    void loop();

private:
    unsigned                    _timer;
    unsigned                    _timerDs;
    unsigned                    _curSensor = 0;
    unsigned                    _dsCount = 0;
    bool                        _ready = false;
    bool                        _enabled = false;
    String                      _name;
    OneWireClass                *_ow = nullptr;
    GyverDS18                   _ds;
    std::vector<MeteoSensor *>  _sensors;
    
    void _sensorsTask();
    void _ds18Task();
};

#endif /* __METEO_HPP__ */