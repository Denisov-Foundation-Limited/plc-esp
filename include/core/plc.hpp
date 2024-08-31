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
    PLC_GPIO_BUZZER,
    PLC_GPIO_MAX
} PlcGpioType;

class PlcClass
{
private:
    String      _name;
    GPIOIface  *_pins[PLC_GPIO_MAX];
    unsigned    _timer = 0;
    unsigned    _alarm = 0;
    unsigned    _buzzer = 0;
    bool        _lastAlarm = false;
    bool        _lastBuzzer = false;

    void alarmBuzzerTask();
public:
    PlcClass();
    void setAlarm(PlcMod mod, bool status);
    void setBuzzer(PlcMod mod, bool status);
    GPIOIface *getPin(PlcGpioType type) const;
    void setPin(PlcGpioType type, GPIOIface *pin);
    const String& getName() const;
    void setName(const String &name);
    void begin();
    void loop();
};

extern PlcClass Plc;

#endif /* __PLC_HPP__ */
