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

#ifndef __I2C_HPP__
#define __I2C_HPP__

#include <Arduino.h>
#include <vector>

#include "core/ifaces/iface.hpp"

#define I2C_MIN_ADDR    0x8
#define I2C_MAX_ADDR    127

typedef enum {
    I2C_PIN_SDA,
    I2C_PIN_SCL,
    I2C_PIN_MAX
} I2cPin;


class I2CIface : public Interface
{
public:
    I2CIface(const String &name, uint8_t sda, uint8_t scl);
    void setPin(I2cPin pin, uint8_t gpio);
    uint8_t getPin(I2cPin pin) const;
    const String &getName() const;
    void findDevices(std::vector<unsigned> &devices);
    IntType getType() const;
    void setName(const String &name);

private:
    uint8_t _pins[I2C_PIN_MAX] = { 0 };
    String  _name;
};

#endif /* __I2C_HPP__ */
