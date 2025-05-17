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

#ifndef __TANK_HPP__
#define __TANK_HPP__

#include <Arduino.h>
#include <vector>

#include "core/ifaces/gpio.hpp"

#define TANK_LEVEL_COUNT    3
#define TANK_COUNT          16
#define TANK_PROCESS_MS     1000

typedef struct {
    size_t  id;
    String  name;
    bool    enabled;
    bool    status;
    uint8_t level;
    GpioPin *valve;
    GpioPin *pump;
    GpioPin *levels[TANK_LEVEL_COUNT];
} Tank;

class TankCtrlClass
{
public:
    TankCtrlClass();
    void setEnabled(bool enabled);
    bool &getEnabled();
    void getEnabledTanks(std::vector<Tank *> &tanks);
    bool setTank(size_t index, Tank *tank);
    void begin();
    void loop();
    void setStatus(Tank *tank, bool status, bool save);

private:
    std::array<Tank, TANK_COUNT>    _tanks;
    bool                            _reading = false;
    unsigned                        _timer;
    unsigned                        _curTank = 0;
    bool                            _enabled = false;

    void _processTank(Tank *tank);
    bool _loadStates();
};

extern TankCtrlClass TankCtrl;

#endif /* __TANK_HPP__ */
