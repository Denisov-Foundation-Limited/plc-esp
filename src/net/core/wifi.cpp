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

#include "net/core/wifi.hpp"
#include "core/plc.hpp"
#include "boards/boards.hpp"

/*********************************************************************/
/*                                                                   */
/*                          PUBLIC FUNCTIONS                         */
/*                                                                   */
/*********************************************************************/

WirelessClass::WirelessClass()
{
}

void WirelessClass::setCreds(const String &ssid, const String &passwd)
{
    _ssid = ssid;
    _passwd = passwd;
}

String &WirelessClass::getSSID()
{
    return _ssid;
}

wl_status_t WirelessClass::getStatus() const
{
    return _status;
}

String &WirelessClass::getPasswd()
{
    return _passwd;
}

void WirelessClass::setEnabled(bool status)
{
    _enabled = status;
}

bool &WirelessClass::getEnabled()
{
    return _enabled;
}

void WirelessClass::setAP(bool status)
{
    _ap = status;
}

bool &WirelessClass::getAP() 
{
    return _ap;
}

String WirelessClass::getIP()
{
    if (_ap)
        return WiFi.softAPIP().toString();
    else
        return WiFi.localIP().toString();
}

void WirelessClass::setHostname(const String &name)
{
    WiFi.setHostname(name.c_str());
}

String WirelessClass::getHostname()
{
    return WiFi.getHostname();
}

void WirelessClass::begin()
{
    if (!_enabled) return;

    if (!Gpio.getPinById(ActiveBoard.wifi.gpio.net, &_statusLed)) {
        Log.error(F("WIFI"), F("GPIO Status led not found"));
    }

    if (_statusLed != nullptr) {
        Gpio.setMode(_statusLed, GPIO_MOD_OUTPUT, GPIO_PULL_NONE);
        Gpio.write(_statusLed, false);
    }

    if (!_ap) {
        WiFi.mode(WIFI_STA);
        WiFi.begin(_ssid, _passwd);
    } else {
        Log.info(F("WIFI"), String(F("Starting Wi-Fi AP: ")) + _ssid);
        WiFi.mode(WIFI_AP);
        WiFi.softAP(_ssid, _passwd);
        Plc.setAlarm(PLC_MOD_WIFI, false);
        Log.info(F("WIFI"), String(F("IP address: ")) + getIP());
    }
}

void WirelessClass::loop()
{
    if (!_enabled) return;

    if (millis() - _timer >= WIFI_DELAY_MS) {
        _timer = millis();
        statusTask();
    }
}

/*********************************************************************/
/*                                                                   */
/*                          PRIVATE FUNCTIONS                        */
/*                                                                   */
/*********************************************************************/

void WirelessClass::statusTask()
{
    if (WiFi.status() != _status) {
        _status = WiFi.status();

        switch (_status)
        {
        case WL_CONNECTED:
            if (_statusLed != nullptr) { Gpio.write(_statusLed, true); }
            Plc.setAlarm(PLC_MOD_WIFI, false);
            Log.info(F("WIFI"), String(F("PLC was connected to SSID: ")) + _ssid);
            Log.info(F("WIFI"), String(F("PLC IP address: ")) + getIP());
            break;

        case WL_CONNECTION_LOST:
            if (_statusLed != nullptr) { Gpio.write(_statusLed, false); }
            Plc.setAlarm(PLC_MOD_WIFI, true);
            Log.info(F("WIFI"), String(F("PLC connection lost to SSID: ")) + _ssid);
            break;

        case WL_IDLE_STATUS:
            break;

        case WL_NO_SHIELD:
            break;

        case WL_NO_SSID_AVAIL:
            if (_statusLed != nullptr) { Gpio.write(_statusLed, false); }
            Plc.setAlarm(PLC_MOD_WIFI, true);
            Log.info(F("WIFI"), String(F("PLC no available SSID: ")) + _ssid);
            break;

        case WL_SCAN_COMPLETED:
            if (_statusLed != nullptr) { Gpio.write(_statusLed, false); }
            Plc.setAlarm(PLC_MOD_WIFI, true);
            Log.info(F("WIFI"), String(F("PLC scan completed for SSID: ")) + _ssid);
            break;
        
        default:
            if (_statusLed != nullptr) { Gpio.write(_statusLed, false); }
            Plc.setAlarm(PLC_MOD_WIFI, true);
            Log.info(F("WIFI"), String(F("PLC has been disconnected from SSID: ")) + _ssid);
            break;
        }
    }
}

WirelessClass Wireless;
