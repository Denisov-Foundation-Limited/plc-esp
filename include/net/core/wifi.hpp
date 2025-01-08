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

#ifndef __WIRELESS_HPP__
#define __WIRELESS_HPP__

#include <Arduino.h>

#include "core/ifaces/gpio.hpp"
#include "utils/log.hpp"
#include "core/plc.hpp"

#ifdef ESP32
#include <WiFi.h>
#else
#include <ESP8266WiFi.h>
#endif

#define WIFI_DELAY_MS   1000

class WirelessClass
{
public:
    WirelessClass();
    void setCreds(const String &ssid, const String &passwd);
    String &getSSID();
    String &getPasswd();
    GpioPin *getStatusLed() const;
    void setStatusLed(GpioPin *gpio);
    void setEnabled(bool status);
    bool &getEnabled();
    void setAP(bool status);
    bool &getAP();
    wl_status_t getStatus() const;
    String getIP();
    void begin();
    void loop();
    void setHostname(const String &name);
    String getHostname();

private:
    String      _ssid;
    String      _passwd;
    GpioPin  *_statusLed = nullptr;
    bool        _enabled = false;
    bool        _ap = true;
    wl_status_t _status = WL_NO_SHIELD;
    unsigned    _timer = 0;

    void statusTask();
};

extern WirelessClass Wireless;

#endif /* __WIRELESS_HPP__ */
