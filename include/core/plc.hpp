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

#ifndef __PLC_HPP__
#define __PLC_HPP__

#include <Arduino.h>

#include "core/ifaces/gpio.hpp"

#define PLC_TIMER_MS    500

typedef enum {
    PLC_MOD_WIFI
} PlcMod;

typedef enum {
    PLC_GPIO_ALARM_LED,
    PLC_GPIO_STATUS_LED,
    PLC_GPIO_BUZZER,
    PLC_GPIO_MAX
} PlcGpioType;

class PlcClass
{
public:
    PlcClass();
    void setAlarm(PlcMod mod, bool status);
    void setBuzzer(PlcMod mod, bool status);
    void setStatus(PlcMod mod, bool status);
    GpioPin *getPin(PlcGpioType type) const;
    void setPin(PlcGpioType type, GpioPin *pin);
    const String& getName() const;
    void setName(const String &name);
    void begin();
    void loop();

private:
    String      _name;
    GpioPin  *_pins[PLC_GPIO_MAX];
    unsigned    _timer = 0;
    unsigned    _alarm = 0;
    unsigned    _buzzer = 0;
    unsigned    _status = 0;
    bool        _lastAlarm = false;
    bool        _lastBuzzer = false;

    void _alarmBuzzerTask();
};

extern PlcClass Plc;

#endif /* __PLC_HPP__ */
