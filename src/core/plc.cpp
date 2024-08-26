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

#include "core/plc.hpp"

/*********************************************************************/
/*                                                                   */
/*                          PRIVATE FUNCTIONS                        */
/*                                                                   */
/*********************************************************************/

void Plc::timerHandler()
{
    if (_alarm > 0) {
        if (!_lastAlarm) {
            _lastAlarm = true;
            if (_pins[PLC_GPIO_ALARM_LED] != nullptr) { _pins[PLC_GPIO_ALARM_LED]->write(true); }
        } else {
            _lastAlarm = false;
            if (_pins[PLC_GPIO_ALARM_LED] != nullptr) { _pins[PLC_GPIO_ALARM_LED]->write(false); }
        }
    }
    if (_buzzer > 0) {
        if (!_lastBuzzer) {
            _lastBuzzer = true;
            if (_pins[PLC_GPIO_BUZZER] != nullptr) { _pins[PLC_GPIO_BUZZER]->write(true); }
        } else {
            _lastBuzzer = false;
            if (_pins[PLC_GPIO_BUZZER] != nullptr) { _pins[PLC_GPIO_BUZZER]->write(false); }
        }
    }
}

/*********************************************************************/
/*                                                                   */
/*                          PUBLIC FUNCTIONS                         */
/*                                                                   */
/*********************************************************************/

Plc::Plc()
{
    _name = F("FutCityPLC");
}

void Plc::setAlarm(PlcMod mod, bool status)
{
    if (status) {
        _alarm |= (1 << mod);
    } else {
        _alarm &= ~(1 << mod);
    }

    if (_alarm == 0) {
        _lastAlarm = false;
        if (_pins[PLC_GPIO_ALARM_LED] != nullptr) { _pins[PLC_GPIO_ALARM_LED]->write(false); }
    }
}

void Plc::setBuzzer(PlcMod mod, bool status)
{
    if (status) {
        _buzzer |= (1 << mod);
    } else {
        _buzzer &= ~(1 << mod);
    }

    if (_buzzer == 0) {
        _lastBuzzer = false;
        if (_pins[PLC_GPIO_BUZZER] != nullptr) { _pins[PLC_GPIO_BUZZER]->write(false); }
    }
}

GpioPin *Plc::getPin(PlcGpioType type) const
{
    return _pins[type];
}

void Plc::setPin(PlcGpioType type, GpioPin *pin)
{
    _pins[type] = pin;
}

const String& Plc::getName() const
{
    return _name;
}

void Plc::setName(const String &name)
{
    _name = name;
}

void Plc::begin()
{
    if (_pins[PLC_GPIO_ALARM_LED] != nullptr) { _pins[PLC_GPIO_ALARM_LED]->write(false); }
    if (_pins[PLC_GPIO_BUZZER] != nullptr) { _pins[PLC_GPIO_BUZZER]->write(false); }
}

void Plc::loop()
{
    if (millis() - _timer >= PLC_TIMER_MS) {
        _timer = millis();
        timerHandler();
    }
}
