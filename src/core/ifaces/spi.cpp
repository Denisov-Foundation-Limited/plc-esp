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

#include "core/ifaces/spi.hpp"

SPIface::SPIface(const String &name, uint8_t miso, uint8_t mosi, uint8_t sck, uint8_t ss, unsigned freq)
{
    _name = name;
    _pins[SPI_PIN_MISO] = miso;
    _pins[SPI_PIN_MOSI] = mosi;
    _pins[SPI_PIN_SCK] = sck;
    _pins[SPI_PIN_SS] = ss;
    _freq = freq;
}

void SPIface::setPin(SpiPin pin, uint8_t gpio)
{
    _pins[pin] = gpio;
}

uint8_t SPIface::getPin(SpiPin pin) const
{
    return _pins[pin];
}

void SPIface::setFrequency(unsigned freq)
{
    _freq = freq;
}

unsigned SPIface::getFrequency() const
{
    return _freq;
}

const String &SPIface::getName() const
{
    return _name;
}

void SPIface::setName(const String &name)
{
    _name = name;
}

IntType SPIface::getType() const
{
    return INT_TYPE_SPI;
}
