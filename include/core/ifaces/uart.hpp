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

#ifndef __UART_HPP__
#define __UART_HPP__

#include <Arduino.h>
#include <vector>
#include "gpio.hpp"

#define UART_BUS_COUNT   PROF_UART_MAX

typedef enum {
    UART_PIN_RX,
    UART_PIN_TX,
    UART_PIN_MAX
} UARTPin;

class UARTClass
{
public:
    void setPin(UARTPin pin, GpioPin *gpio);
    GpioPin *getPin(UARTPin pin) const;
    const String &getName() const;
    unsigned getRate() const;
    void setRate(unsigned rate);
    void setName(const String &name);

private:
    GpioPin     *_pins[UART_PIN_MAX] = { nullptr };
    String      _name;
    unsigned    _rate;
};

#endif /* __UART_HPP__ */
