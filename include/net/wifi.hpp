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

class Wireless
{
private:
    String      _ssid;
    String      _passwd;
    GPIOIface  *_statusLed = nullptr;
    bool        _enabled = true;
    bool        _ap = true;
    wl_status_t _status = WL_DISCONNECTED;
    unsigned    _timer = 0;

    Logger  *_log;
    Plc     *_plc;

    void statusTask();
public:
    Wireless(Logger *log, Plc *plc);
    void setCreds(const String &ssid, const String &passwd);
    const String &getSSID() const;
    const String &getPasswd() const;
    GPIOIface *getStatusLed() const;
    void setStatusLed(GPIOIface *gpio);
    void setEnabled(bool status);
    bool getEnabled() const;
    void setAP(bool status);
    bool getAP() const;
    wl_status_t getStatus() const;
    String getIP() const;
    void begin();
    void loop();
};

#endif /* __WIRELESS_HPP__ */
