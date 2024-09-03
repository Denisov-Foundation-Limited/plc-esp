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

#include "controllers/meteo/meteo.hpp"
#include "controllers/meteo/sensors/ds18b20.hpp"

/*********************************************************************/
/*                                                                   */
/*                          PUBLIC FUNCTIONS                         */
/*                                                                   */
/*********************************************************************/

Meteo::Meteo(const String &name)
{
    _name = name;
}

void Meteo::setEnabled(bool status)
{
    _enabled = status;
}

bool Meteo::getEnabled()
{
    return _enabled;
}

void Meteo::setOneWire(OneWireIface *ow)
{
    _ow = ow;
    _ds.setPin(ow->getPin());
}

OneWireIface *Meteo::getOneWire()
{
    return _ow;
}

void Meteo::addSensor(MeteoSensor *sensor)
{
    if (sensor->getType() == METEO_SENSOR_DS18) {
        auto *s = static_cast<Ds18b20 *>(sensor);
        s->setDSBus(&_ds);
        _sensors.push_back(s);
        Log.info(LOG_MOD_METEO, String(F("Add sensor name: ")) +
                                sensor->getName() +
                                String(F(" type: DS18B20")));
        _dsCount++;
    }
}

const std::vector<MeteoSensor *> &Meteo::getSensors()
{
    return _sensors;
}

MeteoSensor *Meteo::getSensor(const String &name)
{
    for (auto s : _sensors) {
        if (s->getName() == name) {
            return s;
        }
    }
    return nullptr;
}

void Meteo::begin()
{
    if (!_enabled || !_sensors.size()) return;
    Log.info(LOG_MOD_METEO, String(F("Starting Meteo controller ")) + _name +
                            String(F(" with ")) + 
                            String(_sensors.size()) +
                            String(F(" sensors")));
}

void Meteo::loop()
{
    if (!_enabled || !_sensors.size()) return;

    if ((millis() - _timerDs >= METEO_DS_TIMER_MS) && !_ready) {
        _timerDs = millis();
        _ds18Task();
    }

    if ((millis() - _timer >= METEO_SENS_TIMER_MS) && !_ready) {
        _timer = millis();
        _sensorsTask();
    }

    if (_ready) {
        _sensors[_curSensor]->readData();
        if (_curSensor < (_sensors.size() - 1)) {
            _curSensor++;
        } else {
            _curSensor = 0;
            _ready = false;
            _timer = millis();
            _timerDs = millis();
        }
    }
}

CtrlType Meteo::getType()
{
    return CTRL_TYPE_METEO;
}

const String &Meteo::getName()
{
    return _name;
}

void Meteo::setName(const String &name)
{
    _name = name;
}

/*********************************************************************/
/*                                                                   */
/*                          PRIVATE FUNCTIONS                        */
/*                                                                   */
/*********************************************************************/

void Meteo::_sensorsTask()
{
    _ready = true;
}
void Meteo::_ds18Task()
{
    if (_ow == nullptr) {
        return;
    }

    if (_dsCount > 0) {
        _ds.requestTemp();
    }
}
