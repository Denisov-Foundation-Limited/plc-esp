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

#ifndef __SPI_HPP__
#define __SPI_HPP__

#include <Arduino.h>

#include "core/ifaces/iface.hpp"

typedef enum {
    SPI_PIN_MISO,
    SPI_PIN_MOSI,
    SPI_PIN_SCK,
    SPI_PIN_SS,
    SPI_PIN_MAX
} SpiPin;


class SPIface : public Interface
{
public:
    SPIface(const String &name, uint8_t miso, uint8_t mosi, uint8_t sck, uint8_t ss, unsigned freq);
    void setPin(SpiPin pin, uint8_t gpio);
    uint8_t getPin(SpiPin pin) const;
    void setFrequency(unsigned freq);
    unsigned getFrequency() const;
    const String &getName() const;
    void setName(const String &name);
    IntType getType() const;

private:
    uint8_t     _pins[SPI_PIN_MAX] = { 0 };
    String      _name;
    unsigned    _freq;
};

#endif /* __SPI_HPP__ */
