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

#ifndef __EXTENDERS_HPP__
#define __EXTENDERS_HPP__

#include <Arduino.h>
#include <vector>
#include <Wire.h>
#include <Adafruit_BusIO_Register.h>
#include <Adafruit_MCP23X17.h>
#include <Adafruit_MCP23X08.h>

#include "utils/log.hpp"

#define EXT_ADDR_COUNT  8

typedef enum {
    EXT_NOT_USED,
    EXT_ID_1,
    EXT_ID_2,
    EXT_ID_3,
    EXT_ID_4,
    EXT_ID_5,
    EXT_ID_6,
    EXT_ID_7,
    EXT_ID_8
} ExtenderId;

typedef enum {
    EXT_ADDR_1 = 0x20,
    EXT_ADDR_2 = 0x21,
    EXT_ADDR_3 = 0x22,
    EXT_ADDR_4 = 0x23,
    EXT_ADDR_5 = 0x24,
    EXT_ADDR_6 = 0x25,
    EXT_ADDR_7 = 0x26,
    EXT_ADDR_8 = 0x27
} ExtenderAddr;

class Extender {
private:
    ExtenderId          _id;
    uint8_t             _addr;

    Adafruit_MCP23X17   _mcp;
public:
    Extender(ExtenderId id, uint8_t addr);
    uint8_t getAddr() const;
    ExtenderId getID() const;
    bool begin();
    void write(uint8_t pin, bool state);
    bool read(uint8_t pin);
    void setID(ExtenderId id);
    void setAddr(unsigned addr);
};

class ExtendersClass
{
private:
    std::vector<Extender*>   _exts;
    unsigned                _addrs[EXT_ADDR_COUNT] = { EXT_ADDR_1, EXT_ADDR_2, EXT_ADDR_3,
                                                        EXT_ADDR_4, EXT_ADDR_5, EXT_ADDR_6,
                                                        EXT_ADDR_7, EXT_ADDR_8 };

public:
    void addExtender(Extender *ext);
    Extender *getById(ExtenderId id) const;
    std::vector<Extender*> &getExtenders();
    bool isExists(ExtenderId id);
    unsigned getLastFreeAddr() const;
};

extern ExtendersClass Extenders;

#endif /* __EXTENDERS_HPP__ */
