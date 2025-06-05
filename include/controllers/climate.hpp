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

#ifndef __CLIMATE_HPP__
#define __CLIMATE_HPP__

#include <Arduino.h>
#include <vector>

#include "core/ifaces/gpio.hpp"
#include "controllers/ctrl.hpp"
#include "controllers/meteo.hpp"

#define CLIMATE_BUTTON_WAIT_MS  1000
#define CLIMATE_BUTTON_READ_MS  100
#define CLIMATE_TEMP_PROC_MS    1000
#define CLIMATE_ZONE_COUNT      32

typedef enum {
    CLIMATE_TYPE_HEAT,
    CLIMATE_TYPE_COOL
} ClimateType;

typedef struct {
    size_t      id;
    String      name;
    bool        status;
    bool        enabled;
    float       temp;
    float       delta;
    bool        work;
    unsigned    timer;
    bool        reading;
    ClimateType type;
    MeteoSensor *sensor;
    GpioPin     *relay;
    GpioPin     *button;
} ClimateZone;

class ClimateCtrlClass
{
public:
    ClimateCtrlClass();
    bool setZone(size_t id, ClimateZone *zone);
    void setEnabled(bool enabled);
    bool &getEnabled();
    void begin(bool load);
    void loop();
    bool &getStatus(ClimateZone *zone);
    void setStatus(ClimateZone *zone, bool status, bool save);
    void getZones(bool enabled, std::vector<ClimateZone*> &zones);
    void setTemp(ClimateZone *zone, int temp, bool save);
    int getTemp(ClimateZone *zone);
    void setDelta(ClimateZone *zone, unsigned delta, bool save);
    unsigned getDelta(ClimateZone *zone);
    std::array<ClimateZone, CLIMATE_ZONE_COUNT> *getZones();

private:
    std::array<ClimateZone, CLIMATE_ZONE_COUNT>  _zones;
    unsigned                                    _tmrButton;
    unsigned                                    _tmrTemp;
    unsigned                                    _curZone = 0;
    bool                                        _enabled = false;
    bool                                        _reading;
    unsigned                                    _curZone2 = 0;

    void _readButton(ClimateZone *zone);
    void _processTemp(ClimateZone *zone);
    bool _loadStates();
};

extern ClimateCtrlClass ClimateCtrl;

#endif /* __CLIMATE_HPP__ */
