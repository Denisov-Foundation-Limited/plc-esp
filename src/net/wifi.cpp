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

#include "net/wifi.hpp"
#include "core/plc.hpp"

/*********************************************************************/
/*                                                                   */
/*                          PRIVATE FUNCTIONS                        */
/*                                                                   */
/*********************************************************************/

void Wireless::timerHandler()
{
    if ((WiFi.status() != _status) && (!_ap)) {
        _status = WiFi.status();

        switch (_status)
        {
        case WL_CONNECTED:
            if (_statusLed != nullptr) { _statusLed->write(true); }
            _plc->setAlarm(PLC_MOD_WIFI, false);
            _log->info(LOG_MOD_WIFI, String(F("PLC was connected to SSID: ")) + _ssid);
            _log->info(LOG_MOD_WIFI, String(F("PLC IP address: ")) + getIP());
            break;

        case WL_CONNECTION_LOST:
            if (_statusLed != nullptr) { _statusLed->write(false); }
            _plc->setAlarm(PLC_MOD_WIFI, true);
            _log->info(LOG_MOD_WIFI, String(F("PLC connection lost to SSID: ")) + _ssid);
            break;

#ifndef ESP32
        case WL_WRONG_PASSWORD:
            if (_statusLed != nullptr) { _statusLed->write(false); }
            _plc->setAlarm(PLC_MOD_WIFI, true);
            _log->info(LOG_MOD_WIFI, String(F("PLC wrong password to SSID: ")) + _ssid);
            break;
#endif

        case WL_IDLE_STATUS:
            if (_statusLed != nullptr) { _statusLed->write(false); }
            _plc->setAlarm(PLC_MOD_WIFI, true);
            _log->info(LOG_MOD_WIFI, String(F("PLC idle connection to SSID: ")) + _ssid);
            break;

        case WL_NO_SSID_AVAIL:
            if (_statusLed != nullptr) { _statusLed->write(false); }
            _plc->setAlarm(PLC_MOD_WIFI, true);
            _log->info(LOG_MOD_WIFI, String(F("PLC no available SSID: ")) + _ssid);
            break;

        case WL_SCAN_COMPLETED:
            if (_statusLed != nullptr) { _statusLed->write(false); }
            _plc->setAlarm(PLC_MOD_WIFI, true);
            _log->info(LOG_MOD_WIFI, String(F("PLC scan completed for SSID: ")) + _ssid);
            break;
        
        default:
            if (_statusLed != nullptr) { _statusLed->write(false); }
            _plc->setAlarm(PLC_MOD_WIFI, true);
            _log->info(LOG_MOD_WIFI, String(F("PLC has been disconnected from SSID: ")) + _ssid);
            break;
        }
    }
}

/*********************************************************************/
/*                                                                   */
/*                          PUBLIC FUNCTIONS                         */
/*                                                                   */
/*********************************************************************/

Wireless::Wireless(Logger *log, Plc *plc)
{
    _log = log;
    _plc = plc;
    _ssid = F("FutCityPLC");
}

void Wireless::setCreds(const String &ssid, const String &passwd)
{
    _ssid = ssid;
    _passwd = passwd;
}

const String &Wireless::getSSID() const
{
    return _ssid;
}

wl_status_t Wireless::getStatus() const
{
    return _status;
}

const String &Wireless::getPasswd() const
{
    return _passwd;
}

GpioPin *Wireless::getStatusLed() const
{
    return _statusLed;
}

void Wireless::setStatusLed(GpioPin *gpio)
{
    _statusLed = gpio;
}

void Wireless::setEnabled(bool status)
{
    _enabled = status;
}

bool Wireless::getEnabled() const
{
    return _enabled;
}

void Wireless::setAP(bool status)
{
    _ap = status;
}

bool Wireless::getAP() const
{
    return _ap;
}

String Wireless::getIP() const
{
    if (_ap)
        return WiFi.softAPIP().toString();
    else
        return WiFi.localIP().toString();
}

void Wireless::begin()
{
    if (!_enabled) return;

    if (_statusLed != nullptr) { _statusLed->write(false); }

    if (!_ap) {
        _log->info(LOG_MOD_WIFI, String(F("Connecting to SSID: ")) + _ssid);
        WiFi.mode(WIFI_STA);
        WiFi.begin(_ssid, _passwd);
    } else {
        _log->info(LOG_MOD_WIFI, String(F("Starting AP: ")) + _ssid);
        WiFi.mode(WIFI_AP);
        WiFi.softAP(_ssid, _passwd);
        _log->info(LOG_MOD_WIFI, String(F("PLC IP address: ")) + getIP());
        if (_statusLed != nullptr) { _statusLed->write(true); }
        _plc->setAlarm(PLC_MOD_WIFI, false);
    }
}

void Wireless::loop()
{
    if (millis() - _timer >= WIFI_DELAY_MS) {
        _timer = millis();
        timerHandler();
    }
}
