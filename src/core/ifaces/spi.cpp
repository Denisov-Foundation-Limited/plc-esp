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

#include "core/ifaces/spi.hpp"

void SPIInterface::setPin(SpiPin pin, GpioPin *gpio)
{
    _pins[pin] = gpio;
}

GpioPin *SPIInterface::getPin(SpiPin pin) const
{
    return _pins[pin];
}

void SPIInterface::setFrequency(unsigned freq)
{
    _freq = freq;
}

unsigned SPIInterface::getFrequency() const
{
    return _freq;
}

const String &SPIInterface::getName() const
{
    return _name;
}

void SPIInterface::setName(const String &name)
{
    _name = name;
}
