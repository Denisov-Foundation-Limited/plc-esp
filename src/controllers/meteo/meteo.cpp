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

MeteoCtrl::MeteoCtrl(const String &name)
{
    _name = name;
}

void MeteoCtrl::setEnabled(bool status)
{
    _enabled = status;
}

bool MeteoCtrl::getEnabled() const
{
    return _enabled;
}

void MeteoCtrl::setOneWire(OneWireClass *ow)
{
    _ow = ow;
 
}

OneWireClass *MeteoCtrl::getOneWire()
{
    return _ow;
}

void MeteoCtrl::addSensor(MeteoSensor *sensor)
{
    if (sensor->getType() == METEO_SENSOR_DS18) {
        auto *s = static_cast<Ds18b20 *>(sensor);
        s->setDSBus(&_ds);
        _sensors.push_back(s);
        Log.info(F("METEO"), String(F("Add sensor name: ")) +
                                sensor->getName() +
                                String(F(" type: DS18B20")));
        _dsCount++;
    }
}

const std::vector<MeteoSensor *> &MeteoCtrl::getSensors()
{
    return _sensors;
}

MeteoSensor *MeteoCtrl::getSensor(const String &name)
{
    for (auto s : _sensors) {
        if (s->getName() == name) {
            return s;
        }
    }
    return nullptr;
}

void MeteoCtrl::begin()
{
    if (!_enabled || !_sensors.size()) return;
    Log.info(F("METEO"), String(F("Starting Meteo controller ")) + _name +
                            String(F(" with ")) + 
                            String(_sensors.size()) +
                            String(F(" sensors")));
}

void MeteoCtrl::loop()
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

CtrlType MeteoCtrl::getType() const
{
    return CTRL_TYPE_METEO;
}

const String &MeteoCtrl::getName() const
{
    return _name;
}

void MeteoCtrl::setName(const String &name)
{
    _name = name;
}

/*********************************************************************/
/*                                                                   */
/*                          PRIVATE FUNCTIONS                        */
/*                                                                   */
/*********************************************************************/

void MeteoCtrl::_sensorsTask()
{
    _ready = true;
}
void MeteoCtrl::_ds18Task()
{
    if (_ow == nullptr) {
        return;
    }

    if (_dsCount > 0) {
        _ds.requestTemp();
    }
}
