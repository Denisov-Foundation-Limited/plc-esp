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

#ifndef __GPIO_HPP__
#define __GPIO_HPP__

#include <Arduino.h>
#include <vector>

#include "core/ext.hpp"
#include "utils/log.hpp"
#include "core/ifaces/iface.hpp"

typedef enum {
    GPIO_MOD_INPUT,
    GPIO_MOD_OUTPUT
} GpioMode;

typedef enum {
    GPIO_PULL_NONE,
    GPIO_PULL_UP,
    GPIO_PULL_DOWN
} GpioPull;

typedef enum {
    GPIO_TYPE_GEN,
    GPIO_TYPE_RELAY,
    GPIO_TYPE_DINPUT
} GpioType;

class IfGPIO : public Interface
{
public:
    IfGPIO(const String &name, GpioType type, uint8_t pin, GpioMode mode, GpioPull pull, ExtenderId extId, bool extended=false);
    void setPinType(GpioType type);
    GpioType getPinType() const;
    void setPin(uint8_t pin);
    void setPull(GpioPull pull);
    void setMode(GpioMode mode);
    void setExtId(ExtenderId id);
    uint8_t getPin() const;
    GpioMode getMode() const;
    GpioPull getPull() const;
    ExtenderId getExtId() const;
    void write(bool val);
    bool read();
    bool getState();
    const String &getName() const ;
    IfType getType() const;
    void setName(const String &name);
    bool getExtended() const;
    void setExtended(bool state);

private:
    String      _name;
    uint8_t     _pin;
    GpioMode    _mode;
    GpioPull    _pull;
    ExtenderId  _extId;
    GpioType    _type;
    bool        _state = false;
    bool        _extended = false;
};

#endif /* __GPIO_HPP__ */
