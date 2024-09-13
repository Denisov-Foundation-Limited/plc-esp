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

#ifndef __OW_HPP__
#define __OW_HPP__

#include <Arduino.h>
#include <vector>
#include <GyverDS18.h>
#include <OneWire.h>

#include "core/ifaces/iface.hpp"

class OneWireIface : public Interface
{
public:
    OneWireIface(const String &name, uint8_t pin, bool extended=false);
    void setPin(uint8_t gpio);
    uint8_t getPin() const;
    const String &getName() const;
    void setName(const String &name);
    void findAddresses(std::vector<String> &addrs);
    IntType getType() const;
    bool getExtended() const;
    void setExtended(bool state);

private:
    uint8_t _pin;
    String  _name;
    OneWire _bus;
    bool    _extended = false;
};

#endif /* __OW_HPP__ */
