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

#include "core/cli/cliinfo.hpp"

CLIInformer::CLIInformer(Wireless *wifi, Gpio *gpio, Extenders *ext)
{
    _wifi = wifi;
    _gpio = gpio;
    _ext = ext;
}

void CLIInformer::showWiFi()
{
    Serial.println(F("\nWiFi configuration:"));
    Serial.printf("\tEnabled    : %s\n", (_wifi->getEnabled() == true) ? "true" : "false");
    Serial.printf("\tSSID       : %s\n", _wifi->getSSID().c_str());
    Serial.printf("\tPassword   : %s\n", _wifi->getPasswd().c_str());
    Serial.printf("\tAP         : %s\n\n", (_wifi->getAP() == true) ? "on" : "off");
}

void CLIInformer::showWiFiStatus()
{
    String wifiStatus = F("Error");

    if (_wifi->getAP()) {
        wifiStatus = "AP";
    } else {
        switch (_wifi->getStatus())
        {
            case WL_CONNECTED:
                wifiStatus = F("Connected");
                break;
        
            case WL_CONNECTION_LOST:
                wifiStatus = F("Connection lost");
                break;

#ifndef ESP32
            case WL_WRONG_PASSWORD:
                wifiStatus = F("Wrong password");
                break;
#endif

            case WL_NO_SHIELD:
                wifiStatus = F("Card not inserted");
                break;

            case WL_DISCONNECTED:
                wifiStatus = F("Disconnected");
                break;

            case WL_CONNECT_FAILED:
                wifiStatus = F("Connect failed");
                break;

            case WL_IDLE_STATUS:
                wifiStatus = F("Idle");
                break;

            case WL_NO_SSID_AVAIL:
                wifiStatus = F("SSID is not available");
                break;

            case WL_SCAN_COMPLETED:
                wifiStatus = F("Scan completed");
                break;
        }
    }

    Serial.println(F("\nWiFi status:"));
    Serial.printf("\tStatus : %s\n", wifiStatus.c_str());
    Serial.printf("\tIP     : %s\n\n", _wifi->getIP().c_str());
}

void CLIInformer::showGpio()
{
    Serial.println(F("\nGPIO configuration:"));
    Serial.println(F("\tName                   Pin   Type     Pull   Extender"));
    Serial.println(F("\t--------------------   ---   ------   ----   --------"));

    for (auto pin : _gpio->getPins()) {
        String sType, sPull;

        switch (pin->getType()) {
            case GPIO_TYPE_INPUT:
                sType = "input";
                break;

            case GPIO_TYPE_OUTPUT:
                sType = "output";
                break;
        }

        switch (pin->getPull()) {
            case GPIO_PULL_NONE:
                sPull = "none";
                break;

            case GPIO_PULL_UP:
                sPull = "up";
                break;

            case GPIO_PULL_DOWN:
                sPull = "down";
                break;
        }

        Serial.printf("\t%-20s   %-3d   %-6s   %-4s   %-8s\n",
            pin->getName().c_str(), pin->getPin(), sType.c_str(),
            sPull.c_str(),
            (pin->getExtId() == 0) ? "N/S" : String(pin->getExtId()).c_str());
    }
    Serial.println("");
}

void CLIInformer::showGpioStatus()
{
    Serial.println(F("\nGPIO status:"));
    Serial.println(F("\tName                   State"));
    Serial.println(F("\t--------------------   -----"));

    for (auto pin : _gpio->getPins()) {
        Serial.printf("\t%-20s   %-5s\n",
            pin->getName().c_str(),
            (pin->getState() == true) ? "High" : "Low");
    }
    Serial.println("");
}

void CLIInformer::showExtenders()
{
    Serial.println(F("\nExtenders configuration:"));
    Serial.println(F("\tId    Address"));
    Serial.println(F("\t---   -------"));

    for (auto ext : _ext->getExtenders()) {
        Serial.printf("\t%-3d   0x%-X\n", ext->getID(), ext->getAddr());
    }
    Serial.println("");
}

void CLIInformer::showTankStatus()
{
    Serial.println(F("\nTanks configuration:"));
    Serial.println(F("\tId   Name                     Status   Pump   Valve   Level"));
    Serial.println(F("\t--   ----------------------   ------   ----   -----   -----"));

    /*int  id = 1;
    for (auto tank : _tankCtrl->getTanks()) {
        Serial.printf("\t%2d   %22s   %6s   %4d   %5d   %5d\n",
            id, tank.getName().c_str(), tank.getStatus() ? "On" : "Off",
            tank.getPumpPin(), tank.getValvePin(), tank.getLevels().size());
        id++;
    }
    Serial.println("");*/
}
