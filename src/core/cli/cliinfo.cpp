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

#include "core/ifaces/gpio.hpp"
#include "core/ifaces/ow.hpp"
#include "core/ifaces/spi.hpp"
#include "core/ifaces/i2c.hpp"

void CLIInformerClass::showWiFi()
{
    Serial.println(F("\nWiFi configuration:"));
    Serial.printf("\tEnabled    : %s\n", (Wireless.getEnabled() == true) ? "true" : "false");
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
    Serial.println("");
    Serial.println(F("\tName         Type       Pin           Mode      Pull    Ext"));
    Serial.println(F("\t----------   --------   -----------   -------   -----   ----"));

    for (auto iface : Interfaces.getInterfaces()) {
        if (iface->getType() != INT_TYPE_GPIO) {
            continue;
        }

        auto gpio = static_cast<GPIOIface *>(iface);

        String sType, sPull;

        switch (gpio->getMode()) {
            case GPIO_MOD_INPUT:
                sType = "Input";
                break;

            case GPIO_MOD_OUTPUT:
                sType = "Output";
                break;
        }

        switch (gpio->getPull()) {
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

        Serial.printf("\t%-10s   %-8s   %-11d   %-7s   %-5s   %-8s\n",
            gpio->getName().c_str(), F("GPIO"), gpio->getPin(), sType.c_str(),
            sPull.c_str(),
            (gpio->getExtId() == EXT_NOT_USED) ? "CPU" : String(gpio->getExtId()).c_str());
    }

    for (auto iface : Interfaces.getInterfaces()) {
        if (iface->getType() != INT_TYPE_OW) {
            continue;
        }

        auto ow = static_cast<OneWireIface *>(iface);

        Serial.printf("\t%-10s   %-8s   %-11d   %-7s   %-5s   %-8s\n",
            ow->getName().c_str(), F("OneWire"), ow->getPin(), F("N/S"),
            F("N/S"), F("CPU"));
    }

    for (auto iface : Interfaces.getInterfaces()) {
        if (iface->getType() != INT_TYPE_I2C) {
            continue;
        }

        auto i2c = static_cast<I2CIface *>(iface);

        String sPins = String(i2c->getPin(I2C_PIN_SDA)) + "," +
                        String(i2c->getPin(I2C_PIN_SCL));

        Serial.printf("\t%-10s   %-8s   %-11s   %-7s   %-5s   %-8s\n",
            i2c->getName().c_str(), F("I2C"), sPins.c_str(), F("N/S"),
            F("N/S"), F("CPU"));
    }

    for (auto iface : Interfaces.getInterfaces()) {
        if (iface->getType() != INT_TYPE_SPI) {
            continue;
        }

        auto spi = static_cast<SPIface *>(iface);

        String sPins = String(spi->getPin(SPI_PIN_MISO)) + "," +
                        String(spi->getPin(SPI_PIN_MOSI))  + "," +
                        String(spi->getPin(SPI_PIN_SCK))  + "," +
                        String(spi->getPin(SPI_PIN_SS));

        Serial.printf("\t%-10s   %-8s   %-11s   %-7s   %-5s   %-8s\n",
            spi->getName().c_str(), F("SPI"), sPins.c_str(), F("N/S"),
            F("N/S"), F("CPU"));
    }
    Serial.println("");
}

void CLIInformerClass::showInterfacesStatus()
{
    Serial.println("");
    Serial.println(F("\tName         State"));
    Serial.println(F("\t----------   -----"));

    for (auto iface : Interfaces.getInterfaces()) {
        if (iface->getType() != INT_TYPE_GPIO) {
            continue;
        }

        auto gpio = static_cast<GPIOIface *>(iface);

        Serial.printf("\t%-20s   %-5s\n",
            gpio->getName().c_str(),
            (gpio->getState() == true) ? "High" : "Low");
    }
    Serial.println("");
}

void CLIInformerClass::showExtenders()
{
    Serial.println("");
    Serial.println(F("\tId    Address"));
    Serial.println(F("\t---   -------"));

    for (auto ext : Extenders.getExtenders()) {
        Serial.printf("\t%-3d   0x%-X\n", ext->getID(), ext->getAddr());
    }
    Serial.println("");
}

void CLIInformerClass::showTankStatus()
{
    Serial.println("");
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

CLIInformerClass CLIInformer;
