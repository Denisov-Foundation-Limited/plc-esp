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

#ifndef __I2C_HPP__
#define __I2C_HPP__

#include <Arduino.h>
#include <vector>
#include <Wire.h>

#define I2C_BUS_COUNT       2
#define I2C_DEFAULT_SPEED   400000
#define I2C_SCAN_ADDR_FIRST 0x01
#define I2C_SCAN_ADDR_LAST  0x7f

typedef struct {
    uint8_t id;
    uint8_t sda;
    uint8_t scl;
    TwoWire *wire;
    bool    enabled;
} I2cBus;

class I2cClass
{
public:
    bool begin();
    void getI2cBuses(std::vector<I2cBus *> &bus);
    bool getI2cBusById(uint8_t id, I2cBus **bus);
    void findDevices(I2cBus *bus, std::vector<byte> &devs);

private:
    std::array<I2cBus, I2C_BUS_COUNT> _i2c;
};

extern I2cClass I2C;

#endif /* __I2C_HPP__ */
