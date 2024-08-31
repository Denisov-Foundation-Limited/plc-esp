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

void WirelessClass::statusTask()
{
    if ((WiFi.status() != _status) && (!_ap)) {
        _status = WiFi.status();

        switch (_status)
        {
        case WL_CONNECTED:
            if (_statusLed != nullptr) { _statusLed->write(true); }
            Plc.setAlarm(PLC_MOD_WIFI, false);
            Log.info(LOG_MOD_WIFI, String(F("PLC was connected to SSID: ")) + _ssid);
            Log.info(LOG_MOD_WIFI, String(F("PLC IP address: ")) + getIP());
            break;

        case WL_CONNECTION_LOST:
            if (_statusLed != nullptr) { _statusLed->write(false); }
            Plc.setAlarm(PLC_MOD_WIFI, true);
            Log.info(LOG_MOD_WIFI, String(F("PLC connection lost to SSID: ")) + _ssid);
            break;

        case WL_IDLE_STATUS:
            break;

        case WL_NO_SSID_AVAIL:
            if (_statusLed != nullptr) { _statusLed->write(false); }
            Plc.setAlarm(PLC_MOD_WIFI, true);
            Log.info(LOG_MOD_WIFI, String(F("PLC no available SSID: ")) + _ssid);
            break;

        case WL_SCAN_COMPLETED:
            if (_statusLed != nullptr) { _statusLed->write(false); }
            Plc.setAlarm(PLC_MOD_WIFI, true);
            Log.info(LOG_MOD_WIFI, String(F("PLC scan completed for SSID: ")) + _ssid);
            break;
        
        default:
            if (_statusLed != nullptr) { _statusLed->write(false); }
            Plc.setAlarm(PLC_MOD_WIFI, true);
            Log.info(LOG_MOD_WIFI, String(F("PLC has been disconnected from SSID: ")) + _ssid);
            break;
        }
    }
}

/*********************************************************************/
/*                                                                   */
/*                          PUBLIC FUNCTIONS                         */
/*                                                                   */
/*********************************************************************/

WirelessClass::WirelessClass()
{
    _ssid = F("FutCityPLC");
}

void WirelessClass::setCreds(const String &ssid, const String &passwd)
{
    _ssid = ssid;
    _passwd = passwd;
}

const String &WirelessClass::getSSID() const
{
    return _ssid;
}

wl_status_t WirelessClass::getStatus() const
{
    return _status;
}

const String &WirelessClass::getPasswd() const
{
    return _passwd;
}

GPIOIface *WirelessClass::getStatusLed() const
{
    return _statusLed;
}

void WirelessClass::setStatusLed(GPIOIface *gpio)
{
    _statusLed = gpio;
}

void WirelessClass::setEnabled(bool status)
{
    _enabled = status;
}

bool WirelessClass::getEnabled() const
{
    return _enabled;
}

void WirelessClass::setAP(bool status)
{
    _ap = status;
}

bool WirelessClass::getAP() const
{
    return _ap;
}

String WirelessClass::getIP() const
{
    if (_ap)
        return WiFi.softAPIP().toString();
    else
        return WiFi.localIP().toString();
}

void WirelessClass::begin()
{
    if (!_enabled) return;

    if (_statusLed != nullptr) { _statusLed->write(false); }

    if (!_ap) {
        Log.info(LOG_MOD_WIFI, String(F("Connecting to SSID: ")) + _ssid);
        WiFi.mode(WIFI_STA);
        WiFi.begin(_ssid, _passwd);
    } else {
        Log.info(LOG_MOD_WIFI, String(F("Starting AP: ")) + _ssid);
        WiFi.mode(WIFI_AP);
        WiFi.softAP(_ssid, _passwd);
        Log.info(LOG_MOD_WIFI, String(F("PLC IP address: ")) + getIP());
        if (_statusLed != nullptr) { _statusLed->write(true); }
        Plc.setAlarm(PLC_MOD_WIFI, false);
    }
}

void WirelessClass::loop()
{
    if (millis() - _timer >= WIFI_DELAY_MS) {
        _timer = millis();
        statusTask();
    }
}

WirelessClass Wireless;
