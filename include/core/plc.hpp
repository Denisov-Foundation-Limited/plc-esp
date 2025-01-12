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
#include <LM75.h>
#include <LiquidCrystal_I2C.h>

#include "core/ifaces/gpio.hpp"

#define PLC_ALARM_TIMER_MS  500
#define PLC_FAN_TIMER_MS    5000
#define PLC_LCD_TIMER_MS    5000
#define PLC_BUZZER_OFF_MS   100

#define PLC_DEFAULT_NAME    F("FutCityPLC")
#define PLC_DEFAULT_ROW_0   F("     FCPLC      ")
#define PLC_DEFAULT_ROW_1   F("Denisov Fnd Ltd.")

#define PLC_BRD_TEMP_MAX    50
#define PLC_BRD_TEMP_MIN    40

#define PLC_RLY_MAX 8

#define PLC_LCD_ROWS    2
#define PLC_LCD_COLS    16

typedef enum {
    PLC_MOD_WIFI
} PlcMod;

typedef enum {
    PLC_GPIO_ALARM_LED,
    PLC_GPIO_STATUS_LED,
    PLC_GPIO_BUZZER,
    PLC_GPIO_FAN,
    PLC_GPIO_BTN_UP,
    PLC_GPIO_BTN_MIDDLE,
    PLC_GPIO_BTN_DOWN,
    PLC_GPIO_LCD_LIGHT,
    PLC_GPIO_MAX
} PlcGpioType;

typedef enum {
    PLC_TASK_BUZZER,
    PLC_TASK_ALARM,
    PLC_TASK_FAN,
    PLC_TASK_LCD
} PlcTasks;

class PlcClass
{
public:
    PlcClass();
    void setAlarm(PlcMod mod, bool status);
    void setBuzzer(PlcMod mod, bool status);
    void setStatus(PlcMod mod, bool status);
    const String& getName() const;
    void setName(const String &name);
    void setFanEnabled(bool en);
    bool &getFanEnabled();
    bool &getFanStatus();
    float &getBoardTemp();
    void begin();
    void loop();

private:
    String              _name = PLC_DEFAULT_NAME;
    GpioPin             *_pins[PLC_GPIO_MAX];
    GpioPin             *_rlyLed[PLC_RLY_MAX];
    unsigned            _timerAlrm = 0, _timerFan = 0, _timerLCD = 0, _timerBuzzerOff = 0;
    unsigned            _alarm = 0;
    unsigned            _buzzer = 0;
    unsigned            _status = 0;
    bool                _lastAlarm = false;
    bool                _lastBuzzer = false;
    LM75                _tempSensor;
    float               _brdTemp = 0;
    bool                _fanStatus = false;
    bool                _fanEnabled = true;
    LiquidCrystal_I2C   _lcd;
    String              _lcdText[PLC_LCD_ROWS] = { PLC_DEFAULT_ROW_0, PLC_DEFAULT_ROW_1 };
    size_t              _curTask = PLC_TASK_BUZZER;
    bool                _bzrOff = false;

    void _taskAlarmBuzzer();
    void _taskFan();
    void _taskLCD();
    void _taskBuzzerOff();
};

extern PlcClass Plc;

#endif /* __PLC_HPP__ */
