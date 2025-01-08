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

void PlcClass::begin()
{
    if (_pins[PLC_GPIO_ALARM_LED] != nullptr) { Gpio.write(_pins[PLC_GPIO_ALARM_LED], false); }
    if (_pins[PLC_GPIO_BUZZER] != nullptr) { Gpio.write(_pins[PLC_GPIO_BUZZER], false); }
}

void PlcClass::loop()
{
    if (millis() - _timer >= PLC_TIMER_MS) {
        _timer = millis();
        _alarmBuzzerTask();
    }
}

/*********************************************************************/
/*                                                                   */
/*                          PRIVATE FUNCTIONS                        */
/*                                                                   */
/*********************************************************************/

void PlcClass::_alarmBuzzerTask()
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

PlcClass Plc;
