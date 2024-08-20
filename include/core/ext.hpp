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
};

class Extenders
{
private:
    std::vector<Extender*>   _exts;

    Logger *_log;
public:
    Extenders(Logger *log) : _log(log) { }
    void addExtender(Extender *ext);
    Extender *getById(ExtenderId id) const;
    std::vector<Extender*> &getExtenders();
};

#endif /* __EXTENDERS_HPP__ */
