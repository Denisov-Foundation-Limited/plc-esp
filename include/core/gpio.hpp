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

#include "ext.hpp"
#include "utils/log.hpp"

typedef enum {
    GPIO_TYPE_INPUT,
    GPIO_TYPE_OUTPUT
} GpioType;

typedef enum {
    GPIO_PULL_NONE,
    GPIO_PULL_UP,
    GPIO_PULL_DOWN
} GpioPull;

class GpioPin
{
private:
    String      _name;
    uint8_t     _pin;
    GpioType    _type;
    GpioPull    _pull;
    ExtenderId  _extId;

    bool        _state = false;

    Extenders   *_ext;
    Logger      *_log;
public:
    GpioPin() { }
    GpioPin(Logger *log, Extenders *ext, const String &name, uint8_t pin, GpioType type, GpioPull pull, ExtenderId extId);
    const String &getName() const;
    void setName(const String &name);
    void setPin(uint8_t pin);
    void setPull(GpioPull pull);
    void setType(GpioType type);
    void setExtId(ExtenderId id);
    uint8_t getPin() const;
    GpioType getType() const;
    GpioPull getPull() const;
    ExtenderId getExtId() const;
    void write(bool val);
    bool read();
    bool getState();
};

class Gpio
{
private:
    std::vector<GpioPin *>  _pins;

    Extenders   *_ext;
    Logger      *_log;
public:
    Gpio(Logger *log, Extenders *ext);
    const std::vector<GpioPin *>& getPins() const;
    GpioPin *getPin(const String &name) const;
    void addGpio(GpioPin *gpio);
};

#endif /* __GPIO_HPP__ */
