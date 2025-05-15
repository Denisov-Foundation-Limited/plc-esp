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

#ifndef __SECURITY_HPP__
#define __SECURITY_HPP__

#include <Arduino.h>
#include <vector>

#include "core/ifaces/gpio.hpp"
#include "controllers/ctrl.hpp"

#define SECURITY_SENSORS_COUNT  64
#define SECURITY_SENSOR_READ_MS 1000
#define SECURITY_SENSOR_WAIT_MS 100
#define SECURITY_KEYS_COUNT     10
#define SECURITY_KEY_READ_MS    500
#define SECURITY_KEY_WAIT_MS    1500

typedef enum {
    SECURITY_SENSOR_REED,
    SECURITY_SENSOR_PIR
} SecuritySensorType;

typedef struct {
    unsigned    id;
    bool        enabled;
    String      name;
    uint64_t    serial;
} SecurityKey;

typedef struct {
    unsigned            id;
    String              name;
    bool                enabled;
    bool                reading;
    unsigned            timer;
    bool                detected;
    SecuritySensorType  type;
    GpioPin             *pin;
} SecuritySensor;

class SecurityCtrlClass
{
public:
    SecurityCtrlClass();
    void getEnabledSensors(std::vector<SecuritySensor *> &sens);
    void getEnabledKeys(std::vector<SecurityKey *> &keys);
    void setStatus(bool status, bool save);
    bool &getStatus();
    void setEnabled(bool enabled);
    bool &getEnabled();
    bool setSensor(size_t index, SecuritySensor *sensor);
    bool setKey(size_t index, SecurityKey *key);
    void begin();
    void loop();
    void setAlarm(bool alarm);
    GpioPin **getRelay();
    void setRelay(GpioPin *relay);

private:
    std::array<SecuritySensor, SECURITY_SENSORS_COUNT>  _sensors;
    std::array<SecurityKey, SECURITY_KEYS_COUNT>        _keys;
    unsigned    _timer;
    unsigned    _tmrKey;
    unsigned    _tmrKeyWait;
    bool        _enabled;
    bool        _status = false;
    bool        _reading;
    bool        _alarm;
    unsigned    _curSensor = 0;
    unsigned    _curKey = 0;
    bool        _readingKey;
    GpioPin    *_relay;
    bool        _waitKey;

    void _readSensor(SecuritySensor *sensor);
    void _processSensor(SecuritySensor *sensor);
    bool _loadStates();
    bool _checkKey(uint64_t serial, SecurityKey **key);
    void _readKeys();
};

extern SecurityCtrlClass SecurityCtrl;

#endif /* __SECURITY_HPP__ */
