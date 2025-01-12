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
    if (!Gpio.getPinById(ActiveBoard.plc.gpio.buzzer, &_pins[PLC_GPIO_BUZZER])) {
        Log.error(F("PLC"), F("GPIO Buzzer not found"));
    }
    if (!Gpio.getPinById(ActiveBoard.plc.gpio.up, &_pins[PLC_GPIO_BTN_UP])) {
        Log.error(F("PLC"), F("GPIO Button Up not found"));
    }
    if (!Gpio.getPinById(ActiveBoard.plc.gpio.middle, &_pins[PLC_GPIO_BTN_MIDDLE])) {
        Log.error(F("PLC"), F("GPIO Button Middle not found"));
    }
    if (!Gpio.getPinById(ActiveBoard.plc.gpio.down, &_pins[PLC_GPIO_BTN_DOWN])) {
        Log.error(F("PLC"), F("GPIO Button Down not found"));
    }
    if (!Gpio.getPinById(ActiveBoard.plc.gpio.lcd, &_pins[PLC_GPIO_LCD_LIGHT])) {
        Log.error(F("PLC"), F("GPIO LCD light not found"));
    }

    if (!I2C.getI2cBusById(ActiveBoard.plc.temp.i2c, &bus)) {
        Log.error(F("PLC"), F("I2C bus temp not found"));
    } else {
        _tempSensor.begin(ActiveBoard.plc.temp.addr, bus->wire);
        Log.info(F("PLC"), String(F("Board temp sensor inited at bus: ")) +
                String(bus->id) + String(F(" addr: 0x")) +
                String(ActiveBoard.plc.temp.addr, HEX));
    }

    if (!I2C.getI2cBusById(ActiveBoard.plc.lcd.i2c, &bus)) {
        Log.error(F("PLC"), F("I2C bus LCD not found"));
    } else {
        _lcd.begin(ActiveBoard.plc.lcd.addr, bus->wire, PLC_LCD_COLS, PLC_LCD_ROWS);
        Log.info(F("PLC"), String(F("Board LCD inited at bus: ")) +
                String(bus->id) + String(F(" addr: 0x")) +
                String(ActiveBoard.plc.lcd.addr, HEX));
    }

    if (_pins[PLC_GPIO_ALARM_LED] != nullptr) {
        Gpio.setMode(_pins[PLC_GPIO_ALARM_LED], GPIO_MOD_OUTPUT, GPIO_PULL_NONE);
        Gpio.write(_pins[PLC_GPIO_ALARM_LED], false);
    }
    if (_pins[PLC_GPIO_STATUS_LED] != nullptr) {
        Gpio.setMode(_pins[PLC_GPIO_STATUS_LED], GPIO_MOD_OUTPUT, GPIO_PULL_NONE);
        Gpio.write(_pins[PLC_GPIO_STATUS_LED], false);
    }
    if (_pins[PLC_GPIO_BUZZER] != nullptr) {
        Gpio.setMode(_pins[PLC_GPIO_BUZZER], GPIO_MOD_OUTPUT, GPIO_PULL_NONE);
        Gpio.write(_pins[PLC_GPIO_BUZZER], true);
    }
    if (_pins[PLC_GPIO_FAN] != nullptr) {
        Gpio.setMode(_pins[PLC_GPIO_FAN], GPIO_MOD_OUTPUT, GPIO_PULL_NONE);
        Gpio.write(_pins[PLC_GPIO_FAN], false);
    }
    if (_pins[PLC_GPIO_BTN_UP] != nullptr) {
        Gpio.setMode(_pins[PLC_GPIO_BTN_UP], GPIO_MOD_INPUT, GPIO_PULL_UP);
    }
    if (_pins[PLC_GPIO_BTN_MIDDLE] != nullptr) {
        Gpio.setMode(_pins[PLC_GPIO_BTN_MIDDLE], GPIO_MOD_INPUT, GPIO_PULL_UP);
    }
    if (_pins[PLC_GPIO_BTN_DOWN] != nullptr) {
        Gpio.setMode(_pins[PLC_GPIO_BTN_DOWN], GPIO_MOD_INPUT, GPIO_PULL_UP);
    }
    if (_pins[PLC_GPIO_LCD_LIGHT] != nullptr) {
        Gpio.setMode(_pins[PLC_GPIO_LCD_LIGHT], GPIO_MOD_OUTPUT, GPIO_PULL_NONE);
        Gpio.write(_pins[PLC_GPIO_LCD_LIGHT], true);
    }
    for (size_t i = 0; i < PLC_RLY_MAX; i++) {
        if (!Gpio.getPinById(ActiveBoard.plc.gpio.relays[i], &_rlyLed[i])) {
            Log.error(F("PLC"), String(F("GPIO Relay LED #")) + String(i+1) +  String(F(" not found")));
        }
        if (_rlyLed[i] != nullptr) { Gpio.setMode(_rlyLed[i], GPIO_MOD_OUTPUT, GPIO_PULL_NONE); }
    }

    _taskLCD();
}

void PlcClass::loop()
{
    switch (_curTask) {
        case PLC_TASK_BUZZER:
            if (millis() - _timerAlrm >= PLC_BUZZER_OFF_MS) {
                _timerBuzzerOff = millis();
                _taskBuzzerOff();
            }
            _curTask++;
            break;

        case PLC_TASK_ALARM:
            if (millis() - _timerAlrm >= PLC_ALARM_TIMER_MS) {
                _timerAlrm = millis();
                _taskAlarmBuzzer();
            }
            _curTask++;
            break;

        case PLC_TASK_FAN:
            if (millis() - _timerFan >= PLC_FAN_TIMER_MS) {
                _timerFan = millis();
                _taskFan();
            }
            _curTask++;
            break;

        case PLC_TASK_LCD:
            if (millis() - _timerLCD >= PLC_LCD_TIMER_MS) {
                _timerLCD = millis();
                _taskLCD();
            }
            _curTask = PLC_TASK_BUZZER;
            break;
    }
}

void PlcClass::setFanEnabled(bool en)
{
    _fanEnabled = en;
    if (!en) {
        if (_pins[PLC_GPIO_FAN] != nullptr) { Gpio.write(_pins[PLC_GPIO_FAN], false); }
        Log.info(F("PLC"), F("FAN status changed to OFF"));
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

void PlcClass::_taskLCD()
{
    _lcd.clear();
    _lcd.setCursor(0, 0);
    _lcd.println(_lcdText[0].c_str());
    _lcd.setCursor(0, 1);
    _lcd.println(_lcdText[1].c_str());
}

void PlcClass::_taskBuzzerOff()
{
    if (_pins[PLC_GPIO_BUZZER] != nullptr && !_bzrOff) {
        Gpio.write(_pins[PLC_GPIO_BUZZER], false);
        _bzrOff = true;
    }
}

PlcClass Plc;
