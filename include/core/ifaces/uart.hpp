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

#ifndef __UART_HPP__
#define __UART_HPP__

#include <Arduino.h>
#include <vector>

#include "core/ifaces/iface.hpp"

typedef enum {
    UART_PIN_RX,
    UART_PIN_TX,
    UART_PIN_MAX
} UARTPin;

class UARTIface : public Interface
{
public:
    UARTIface(const String &name, uint8_t rx, uint8_t tx, unsigned rate);
    void setPin(UARTPin pin, uint8_t gpio);
    uint8_t getPin(UARTPin pin) const;
    const String &getName() const;
    unsigned getRate() const;
    void setRate(unsigned rate);
    IntType getType() const;
    void setName(const String &name);

private:
    uint8_t     _pins[UART_PIN_MAX] = { 0 };
    String      _name;
    unsigned    _rate;
};

#endif /* __UART_HPP__ */
