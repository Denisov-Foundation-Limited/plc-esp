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

IfSPI::IfSPI(const String &name, uint8_t miso, uint8_t mosi, uint8_t sck, uint8_t ss, unsigned freq, bool extended)
{
    _name = name;
    _pins[SPI_PIN_MISO] = miso;
    _pins[SPI_PIN_MOSI] = mosi;
    _pins[SPI_PIN_SCK] = sck;
    _pins[SPI_PIN_SS] = ss;
    _freq = freq;
    _extended = extended;
}

void IfSPI::setPin(SpiPin pin, uint8_t gpio)
{
    _pins[pin] = gpio;
}

bool IfSPI::getExtended() const
{
    return _extended;
}

void IfSPI::setExtended(bool state)
{
    _extended = state;
}

uint8_t IfSPI::getPin(SpiPin pin) const
{
    return _pins[pin];
}

void IfSPI::setFrequency(unsigned freq)
{
    _freq = freq;
}

unsigned IfSPI::getFrequency() const
{
    return _freq;
}

const String &IfSPI::getName() const
{
    return _name;
}

void IfSPI::setName(const String &name)
{
    _name = name;
}

IfType IfSPI::getType() const
{
    return IF_TYPE_SPI;
}
