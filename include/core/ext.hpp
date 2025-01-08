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

#ifndef __EXTENDERS_HPP__
#define __EXTENDERS_HPP__

#include <Arduino.h>
#include <vector>
#include <Wire.h>
#include <Adafruit_BusIO_Register.h>
#include <Adafruit_MCP23X17.h>

#include "utils/log.hpp"
#include <core/ifaces/i2c.hpp>

#define EXT_COUNT  16

typedef enum {
    EXT_TYPE_MCP_16,
    EXT_TYPE_MCP_8
} ExtenderType;

typedef struct {
    uint8_t             id;
    I2cBus              *i2c;
    uint16_t            addr;
    Adafruit_MCP23X17   mcp;
    bool                enabled;
    bool                active;
} Extender;

class ExtendersClass
{
public:
    bool begin();
    void write(Extender *ext, uint16_t pin, bool state);
    bool read(Extender *ext, uint16_t pin);
    void setPinMode(Extender *ext, uint16_t pin, uint8_t mode);
    void getExtenders(std::vector<Extender *> &ext);
    bool getExtenderById(uint8_t id, Extender **ext);

private:
    std::array<Extender, EXT_COUNT>   _ext;
};

extern ExtendersClass Extenders;

#endif /* __EXTENDERS_HPP__ */
