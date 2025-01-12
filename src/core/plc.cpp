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

#include "core/plc.hpp"
#include "boards/boards.hpp"

/*********************************************************************/
/*                                                                   */
/*                          PUBLIC FUNCTIONS                         */
/*                                                                   */
/*********************************************************************/

PlcClass::PlcClass()
{
    _name = F("FutCityPLC");
}

void PlcClass::setAlarm(PlcMod mod, bool status)
{
    if (status) {
        _alarm |= (1 << mod);
    } else {
        _alarm &= ~(1 << mod);
    }

    if (_alarm == 0) {
        _lastAlarm = false;
        if (_pins[PLC_GPIO_ALARM_LED] != nullptr) { Gpio.write(_pins[PLC_GPIO_ALARM_LED], false); }
    }
}

void PlcClass::setBuzzer(PlcMod mod, bool status)
{
    if (status) {
        _buzzer |= (1 << mod);
    } else {
        _buzzer &= ~(1 << mod);
    }

    if (_buzzer == 0) {
        _lastBuzzer = false;
        if (_pins[PLC_GPIO_BUZZER] != nullptr) { Gpio.write(_pins[PLC_GPIO_BUZZER], false); }
    }
}

void PlcClass::setStatus(PlcMod mod, bool status)
{
    if (status) {
        _status |= (1 << mod);
    } else {
        _status &= ~(1 << mod);
    }

    if (_status == 0) {
        if (_pins[PLC_GPIO_STATUS_LED] != nullptr) { Gpio.write(_pins[PLC_GPIO_STATUS_LED], false); }
    } else {
        if (_pins[PLC_GPIO_STATUS_LED] != nullptr) { Gpio.write(_pins[PLC_GPIO_STATUS_LED], true); }
    }
}

GpioPin *PlcClass::getPin(PlcGpioType type) const
{
    return _pins[type];
}

void PlcClass::setPin(PlcGpioType type, GpioPin *pin)
{
    _pins[type] = pin;
}

const String& PlcClass::getName() const
{
    return _name;
}

void PlcClass::setName(const String &name)
{
    _name = name;
}

void PlcClass::setTempAddr(uint8_t addr, TwoWire *bus)
{
    _tempSensor.begin(addr, bus);
}

void PlcClass::begin()
{
    I2cBus *bus = nullptr;

    if (!Gpio.getPinById(ActiveBoard.plc.gpio.fan, &_pins[PLC_GPIO_FAN])) {
        Log.error(F("PLC"), F("GPIO FAN not found"));
    }

    if (!Gpio.getPinById(ActiveBoard.plc.gpio.alarm, &_pins[PLC_GPIO_ALARM_LED])) {
        Log.error(F("PLC"), F("GPIO Alarm not found"));
    }

    if (!Gpio.getPinById(ActiveBoard.plc.gpio.status, &_pins[PLC_GPIO_STATUS_LED])) {
        Log.error(F("PLC"), F("GPIO Status not found"));
    }

    if (!I2C.getI2cBusById(ActiveBoard.plc.temp.i2c, &bus)) {
        Log.error(F("PLC"), F("I2C bus temp not found"));
    } else {
        setTempAddr(ActiveBoard.plc.temp.addr, bus->wire);
        Log.info(F("PLC"), "Board temp sensor inited at bus: " + String(bus->id) + " addr: 0x" + String(ActiveBoard.plc.temp.addr, HEX));
    }

    if (_pins[PLC_GPIO_ALARM_LED] != nullptr) { Gpio.write(_pins[PLC_GPIO_ALARM_LED], false); }
    if (_pins[PLC_GPIO_BUZZER] != nullptr) { Gpio.write(_pins[PLC_GPIO_BUZZER], false); }
}

void PlcClass::loop()
{
    if (millis() - _timerAlrm >= PLC_ALARM_TIMER_MS) {
        _timerAlrm = millis();
        _taskAlarmBuzzer();
    }
    if (millis() - _timerFan >= PLC_FAN_TIMER_MS) {
        _timerFan = millis();
        _taskFan();
    }
}

void PlcClass::setFanEnabled(bool en)
{
    _fanEnabled = en;
    if (!en) {
        if (_pins[PLC_GPIO_FAN] != nullptr) { Gpio.write(_pins[PLC_GPIO_FAN], false); }
        Log.info(F("PLC"), "FAN status changed to OFF");
        _fanStatus = false;
    }
}

bool &PlcClass::getFanEnabled()
{
    return _fanEnabled;
}

bool &PlcClass::getFanStatus()
{
    return _fanStatus;
}

float &PlcClass::getBoardTemp()
{
    return _brdTemp;
}

/*********************************************************************/
/*                                                                   */
/*                          PRIVATE FUNCTIONS                        */
/*                                                                   */
/*********************************************************************/

void PlcClass::_taskAlarmBuzzer()
{
    if (_alarm > 0) {
        if (!_lastAlarm) {
            _lastAlarm = true;
            if (_pins[PLC_GPIO_ALARM_LED] != nullptr) { Gpio.write(_pins[PLC_GPIO_ALARM_LED], true); }
        } else {
            _lastAlarm = false;
            if (_pins[PLC_GPIO_ALARM_LED] != nullptr) { Gpio.write(_pins[PLC_GPIO_ALARM_LED], false); }
        }
    }
    if (_buzzer > 0) {
        if (!_lastBuzzer) {
            _lastBuzzer = true;
            if (_pins[PLC_GPIO_BUZZER] != nullptr) { Gpio.write(_pins[PLC_GPIO_BUZZER], true); }
        } else {
            _lastBuzzer = false;
            if (_pins[PLC_GPIO_BUZZER] != nullptr) { Gpio.write(_pins[PLC_GPIO_BUZZER], false); }
        }
    }
}

void PlcClass::_taskFan()
{
    _brdTemp = _tempSensor.getTemperature();

    if (!_fanEnabled) {
        return;
    }

    if (_brdTemp > PLC_BRD_TEMP_MAX && !_fanStatus) {
        if (_pins[PLC_GPIO_FAN] != nullptr) { 
            Gpio.setMode(_pins[PLC_GPIO_FAN], GPIO_MOD_OUTPUT, GPIO_PULL_NONE);
            Gpio.write(_pins[PLC_GPIO_FAN], true);
            _fanStatus = true;
            Log.info(F("PLC"), String(F("FAN status changed to ON. Temp: ")) +
                    String(_brdTemp) + String(F(" > MaxTemp: ")) +
                    String(PLC_BRD_TEMP_MAX));
        }
    } else if (_brdTemp < PLC_BRD_TEMP_MIN && _fanStatus) {
        if (_pins[PLC_GPIO_FAN] != nullptr) {
            Gpio.write(_pins[PLC_GPIO_FAN], false);
            _fanStatus = false;
            Log.info(F("PLC"), String(F("FAN status changed to OFF. Temp: ")) +
                    String(_brdTemp) + String(F(" < MinTemp: ")) +
                    String(PLC_BRD_TEMP_MIN));
        }
    }
}

PlcClass Plc;
