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

#ifndef __SPI_HPP__
#define __SPI_HPP__

#include <Arduino.h>

#include "gpio.hpp"

#define SPI_BUS_COUNT   PROF_SPI_MAX

typedef enum {
    SPI_PIN_MISO,
    SPI_PIN_MOSI,
    SPI_PIN_SCK,
    SPI_PIN_SS,
    SPI_PIN_MAX
} SpiPin;

class SPIInterface
{
public:
    void setPin(SpiPin pin, GpioPin *gpio);
    GpioPin *getPin(SpiPin pin) const;
    void setFrequency(unsigned freq);
    unsigned getFrequency() const;
    const String &getName() const;
    void setName(const String &name);

private:
    GpioPin     *_pins[SPI_PIN_MAX] = { nullptr };
    String      _name;
    unsigned    _freq;
};

#endif /* __SPI_HPP__ */
