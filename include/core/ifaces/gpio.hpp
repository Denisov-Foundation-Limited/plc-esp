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

#ifndef __GPIO_HPP__
#define __GPIO_HPP__

#include <Arduino.h>
#include <array>

#include <core/ext.hpp>
#include "utils/log.hpp"

#define GPIO_PINS_COUNT (128 * 2 + 16)

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
    GPIO_TYPE_GENERIC,
    GPIO_TYPE_INPUT,
    GPIO_TYPE_RELAY,
    GPIO_TYPE_SENSOR,
    GPIO_TYPE_BUZZER
} GpioType;

typedef struct {
    uint16_t    id;
    uint8_t     pin;
    GpioType    type;
    GpioMode    mode;
    GpioPull    pull;
    Extender    *ext;
    bool        state;
    bool        enabled;
} GpioPin;

class GpioClass
{
public:
    bool begin();
    void write(GpioPin *pin, bool val);
    bool read(GpioPin *pin);
    bool getState(GpioPin *pin);
    bool getPinById(uint16_t id, GpioPin **pin);
    void getPins(std::vector<GpioPin *> &pins);
    void getPinsByType(GpioType type, std::vector<GpioPin *> &pins);
    void setMode(GpioPin *pin, GpioMode mode, GpioPull pull);

private:
    std::array<GpioPin, GPIO_PINS_COUNT> _pins;
    void _beginPin(GpioPin *pin);
};

extern GpioClass Gpio;

#endif /* __GPIO_HPP__ */
