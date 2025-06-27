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

#ifndef __STACK_HPP__
#define __STACK_HPP__

#include <Arduino.h>
#include <vector>

#include "HTTPClient.h"

#define STACK_UNIT_PROC_MS    5000
#define STACK_UNIT_COUNT      8

typedef struct {
    size_t  id;
    String  name;
    String  ip;
    bool    enabled;
    bool    online;
} StackUnit;

class StackClass
{
public:
    StackClass();
    bool setUnit(size_t id, StackUnit *unit);
    void setEnabled(bool enabled);
    bool getEnabled() const;
    void begin();
    void loop();
    void getUnits(bool enabled, std::vector<StackUnit*> &units);

private:
    std::array<StackUnit, STACK_UNIT_COUNT> _units;
    unsigned                                _timer;
    unsigned                                _curUnit = 0;
    bool                                    _enabled = false;
    bool                                    _reading;
    HTTPClient                              _client;

    void _processUnit(StackUnit *unit);
};

extern StackClass Stack;

#endif /* __STACK_HPP__ */
