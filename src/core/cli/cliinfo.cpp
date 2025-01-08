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

#include "core/cli/cliinfo.hpp"

#include "core/ifaces/gpio.hpp"
#include "core/ifaces/ow.hpp"
#include "core/ifaces/spi.hpp"
#include "core/ifaces/i2c.hpp"
#include "core/ifaces/uart.hpp"
#include "controllers/ctrls.hpp"
#include "controllers/meteo/meteo.hpp"
#include "controllers/meteo/sensors/ds18b20.hpp"
#include "net/tgbot.hpp"

void CLIInformerClass::showWiFi()
{
    Serial.println(F("\nWiFi configuration:"));
    Serial.printf("\tStatus     : %s\n", (Wireless.getEnabled() == true) ? F("Enabled") : F("Disabled"));
    Serial.printf("\tSSID       : %s\n", Wireless.getSSID().c_str());
    Serial.printf("\tPassword   : %s\n", Wireless.getPasswd().c_str());
    Serial.printf("\tAP         : %s\n\n", (Wireless.getAP() == true) ? "on" : "off");
}

void CLIInformerClass::showWiFiStatus()
{
    String wifiStatus = F("Error");

    if (Wireless.getAP()) {
        wifiStatus = "AP";
    } else {
        switch (Wireless.getStatus())
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
    Serial.printf("\tIP     : %s\n\n", Wireless.getIP().c_str());
}

void CLIInformerClass::showInterfaces()
{
    std::vector<GpioPin *> pins;

    Gpio.getPins(pins);

    Serial.println("");
    Serial.println(F("\tName         Type       Pin           Mode      Pull    Ext"));
    Serial.println(F("\t----------   --------   -----------   -------   -----   ----"));

    for (auto pin : pins) {
        String sType, sMode, sPull;

        switch (pin->mode) {
            case GPIO_MOD_INPUT:
                sMode = "Input";
                break;

            case GPIO_MOD_OUTPUT:
                sMode = "Output";
                break;
        }

        switch (pin->pull) {
            case GPIO_PULL_NONE:
                sPull = "None";
                break;

            case GPIO_PULL_UP:
                sPull = "Up";
                break;

            case GPIO_PULL_DOWN:
                sPull = "Down";
                break;
        }

        Serial.printf("\t%-10d   %-8s   %-11d   %-7s   %-5s   %-8s\n",
            pin->id, sType.c_str(), pin->pin, sMode.c_str(),
            sPull.c_str(),
            (pin->ext == nullptr) ? "CPU" : ("Ext" + String(pin->ext->id)));
    }
    Serial.println("");
}

void CLIInformerClass::showInterfacesStatus()
{
    std::vector<GpioPin *> pins;

    Gpio.getPins(pins);

    Serial.println("");
    Serial.println(F("\tName         State"));
    Serial.println(F("\t----------   -----"));

    for (auto pin : pins) {
        Serial.printf("\t%-20d   %-5s\n",
            pin->id,
            (pin->state == true) ? "High" : "Low");
    }
    Serial.println("");
}

void CLIInformerClass::showExtenders()
{
    std::vector<Extender *> exts;

    Extenders.getExtenders(exts);

    Serial.println("");
    Serial.println(F("\tId    Address"));
    Serial.println(F("\t---   -------"));

    for (auto ext : exts) {
        Serial.printf("\t%-3d   0x%-X\n", ext->id, ext->addr);
    }
    Serial.println("");
}

void CLIInformerClass::showControllers()
{
    Serial.println("");
    Serial.println(F("\tName           Type       Status "));
    Serial.println(F("\t------------   --------   ---------"));

    /*for (auto *ctrl : Controllers.getControllers()) {
        String sType;

        switch (ctrl->getType()) {
            case CTRL_TYPE_METEO:
                sType = F("Meteo");
                break;

            case CTRL_TYPE_SOCKET:
                sType = F("Socket");
                break;

            case CTRL_TYPE_SECURITY:
                sType = F("Security");
                break;

            case CTRL_TYPE_TANK:
                sType = F("Tank");
                break;
        }

        Serial.printf("\t%-12s   %-8s   %-9s\n",
            ctrl->getName().c_str(),
            sType, ctrl->getEnabled() ? F("Enabled") : F("Disabled"));
    }

    Serial.println("");*/
}

void CLIInformerClass::showMeteo()
{
    /*Serial.println("");
    Serial.println(F("\tName           OneWire   Sensors   Status  "));
    Serial.println(F("\t------------   -------   -------   --------"));

    for (auto *ctrl : Controllers.getControllers()) {
        if (ctrl->getType() != CTRL_TYPE_METEO)
            continue;

        auto *meteo = static_cast<MeteoCtrl *>(ctrl);

        String sOw;
        
        if (meteo->getOneWire() != nullptr) {
            //sOw = meteo->getOneWire()->getName();
        } else {
            sOw = F("None");
        }

        Serial.printf("\t%-12s   %-7s   %-7d   %-8s\n",
            meteo->getName().c_str(), sOw.c_str(), meteo->getSensors().size(),
            meteo->getEnabled() ? F("Enabled") : F("Disabled"));
    }

    Serial.println("");*/
}

void CLIInformerClass::showMeteoStatus()
{
    /*for (auto *ctrl : Controllers.getControllers()) {
        if (ctrl->getType() != CTRL_TYPE_METEO)
            continue;

        auto *meteo = static_cast<MeteoCtrl *>(ctrl);

        Serial.println("\n" + ctrl->getName() + ":\n");
        
        Serial.println(F("\tName           Type       Temperature   Humidity   Pressure   Id"));
        Serial.println(F("\t------------   --------   -----------   --------   --------   ------------------"));

        for (auto *sensor : meteo->getSensors()) {
            String  sType, sId;
            switch (sensor->getType()) {
                case METEO_SENSOR_DS18:
                    sType = F("DS18B20");
                    sId = static_cast<Ds18b20 *>(sensor)->getId();
                    break;
                case METEO_SENSOR_DHT22:
                    sType = F("DHT22");
                    sId = F("N/S");
                    break;
                case METEO_SENSOR_BME280:
                    sType = F("BME280");
                    sId = F("N/S");
                    break;
            }
            Serial.printf("\t%-12s   %-8s   %-11.2f   %-8.2f   %-8.2f   %-18s\n", sensor->getName().c_str(),
                sType, sensor->getTemperature(), sensor->getHumidity(), sensor->getPressure(), sId.c_str());
        }
    }

    Serial.println("");*/
}

void CLIInformerClass::showOneWire()
{
    unsigned                    i = 0;
    std::vector<OneWireBus *>   buses;

    OneWireIf.getOWBuses(buses);

    Serial.println("");
    Serial.println(F("\tBus   Num   Serial"));
    Serial.println(F("\t---   ---   ------------------"));

    for (auto ow : buses) {
        std::vector<String> addrs;
        OneWireIf.findDevices(ow, addrs);

        i = 1;
        for (auto addr : addrs) {
            addr.toUpperCase();
            Serial.printf("\t%-3d   %-3d   %-18s\n", ow->id, i, addr.c_str());
            i++;
        }
    }
    Serial.println("");
}

void CLIInformerClass::showI2C()
{
    unsigned                i = 0;
    std::vector<I2cBus *>   buses;

    I2C.getI2cBuses(buses);

    Serial.println("");
    Serial.println(F("\tBus   Num   Address"));
    Serial.println(F("\t---   ---   -------"));
    
    for (auto *i2c : buses) {    
        std::vector<byte> devs;
        I2C.findDevices(i2c, devs);

        i = 0;
        for (auto dev : devs) {
            i++;
            Serial.printf("\t%-3d   %-3d   %-18s\n", i2c->id, i, ("0x" + String(dev, HEX)).c_str());
        }
    }
    Serial.println("");
}

void CLIInformerClass::showTgBot()
{
    unsigned                i = 1;
    std::vector<TgUser *>   users;

    Serial.println("");
    Serial.println("Telegram bot configurations:");
    Serial.printf("\tStatus : %s\n", TgBot.getEnabled() ? F("Enabled") : F("Disabled"));
    Serial.printf("\tToken  : %s\n", TgBot.getToken().c_str());
    Serial.printf("\tPeriod : %d\n", TgBot.getPollPeriod());
    String sMode = "";
    switch (TgBot.getPollMode()) {
        case fb::Poll::Async:
            sMode = F("Async");
            break;
        case fb::Poll::Sync:
            sMode = F("Sync");
            break;
        case fb::Poll::Long:
            sMode = F("Long");
            break;
    }
    Serial.printf("\tMode   : %s\n", sMode.c_str());

    Serial.println(F("\nUsers:\n"));
    Serial.println(F("\tId    Name           ChatId      Notify   Admin"));
    Serial.println(F("\t---   ------------   ---------   ------   -----"));
    
    TgBot.getEnabledUsers(users);
    for (auto *user : users) {
        Serial.printf("\t%-3d   %-12s   %-9d   %-6s   %-5s\n", i, user->name.c_str(), user->chatId,
                    user->notify ? F("On") : F("Off"), user->admin ? F("True") : F("False"));
        i++;
    }
    Serial.println("");
}

CLIInformerClass CLIInformer;
