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

#ifndef __OW_HPP__
#define __OW_HPP__

#include <Arduino.h>
#include <vector>
#include <GyverDS18.h>
#include <OneWire.h>

#include "gpio.hpp"

#define OW_BUS_COUNT   2

typedef struct {
    uint8_t id;
    OneWire ow;
    uint8_t pin;
    bool    enabled;
} OneWireBus;

class OneWireClass
{
public:
    bool begin();
    void getOWBuses(std::vector<OneWireBus *> &buses);
    bool getOWBusById(uint8_t id, OneWireBus **bus);
    void findDevices(OneWireBus *bus, std::vector<String> &addrs);

private:
    std::array<OneWireBus, OW_BUS_COUNT> _ow;
};

extern OneWireClass OneWireIf;

#endif /* __OW_HPP__ */
