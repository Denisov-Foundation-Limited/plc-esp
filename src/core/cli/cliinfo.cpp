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
    Serial.println("");
    Serial.println(F("\tName         Type       Pin           Mode      Pull    Ext"));
    Serial.println(F("\t----------   --------   -----------   -------   -----   ----"));

    for (auto iface : Interfaces.getInterfaces()) {
        if (iface->getType() != INT_TYPE_GPIO && iface->getType() != INT_TYPE_DIGITAL_INPUT && iface->getType() != INT_TYPE_RELAY) {
            continue;
        }

        auto gpio = static_cast<IfGPIO *>(iface);

        String sType, sMode, sPull;

        switch (iface->getType()) {
            case INT_TYPE_GPIO:
                sType = F("GPIO");
                break;

            case INT_TYPE_DIGITAL_INPUT:
                sType = F("Input");
                break;
            
            case INT_TYPE_RELAY:
                sType = F("Relay");
                break;
        }

        switch (gpio->getMode()) {
            case GPIO_MOD_INPUT:
                sMode = "Input";
                break;

            case GPIO_MOD_OUTPUT:
                sMode = "Output";
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
            gpio->getName().c_str(), sType.c_str(), gpio->getPin(), sMode.c_str(),
            sPull.c_str(),
            (gpio->getExtId() == EXT_NOT_USED) ? "CPU" : String(gpio->getExtId()).c_str());
    }

    for (auto iface : Interfaces.getInterfaces()) {
        if (iface->getType() != INT_TYPE_OW) {
            continue;
        }

        auto ow = static_cast<IfOneWire *>(iface);

        Serial.printf("\t%-10s   %-8s   %-11d   %-7s   %-5s   %-8s\n",
            ow->getName().c_str(), F("OneWire"), ow->getPin(), F("N/S"),
            F("N/S"), F("CPU"));
    }

    for (auto iface : Interfaces.getInterfaces()) {
        if (iface->getType() != INT_TYPE_I2C) {
            continue;
        }

        auto i2c = static_cast<IfI2C *>(iface);

        String sPins = String(i2c->getPin(I2C_PIN_SDA)) + "," +
                        String(i2c->getPin(I2C_PIN_SCL));

        Serial.printf("\t%-10s   %-8s   %-11s   %-7s   %-5s   %-8s\n",
            i2c->getName().c_str(), F("I2C"), sPins.c_str(), F("N/S"),
            F("N/S"), F("CPU"));
    }

    for (auto iface : Interfaces.getInterfaces()) {
        if (iface->getType() != INT_TYPE_UART) {
            continue;
        }

        auto uart = static_cast<IfUART *>(iface);

        String sPins = String(uart->getPin(UART_PIN_RX)) + "," +
                        String(uart->getPin(UART_PIN_TX));

        Serial.printf("\t%-10s   %-8s   %-11s   %-7s   %-5s   %-8s\n",
            uart->getName().c_str(), F("UART"), sPins.c_str(), F("N/S"),
            F("N/S"), F("CPU"));
    }

    for (auto iface : Interfaces.getInterfaces()) {
        if (iface->getType() != INT_TYPE_SPI) {
            continue;
        }

        auto spi = static_cast<IfSPI *>(iface);

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

        auto gpio = static_cast<IfGPIO *>(iface);

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

void CLIInformerClass::showControllers()
{
    Serial.println("");
    Serial.println(F("\tName           Type       Status "));
    Serial.println(F("\t------------   --------   ---------"));

    for (auto *ctrl : Controllers.getControllers()) {
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

    Serial.println("");
}

void CLIInformerClass::showMeteo()
{
    Serial.println("");
    Serial.println(F("\tName           OneWire   Sensors   Status  "));
    Serial.println(F("\t------------   -------   -------   --------"));

    for (auto *ctrl : Controllers.getControllers()) {
        if (ctrl->getType() != CTRL_TYPE_METEO)
            continue;

        auto *meteo = static_cast<MeteoCtrl *>(ctrl);

        String sOw;
        
        if (meteo->getOneWire() != nullptr) {
            sOw = meteo->getOneWire()->getName();
        } else {
            sOw = F("None");
        }

        Serial.printf("\t%-12s   %-7s   %-7d   %-8s\n",
            meteo->getName().c_str(), sOw.c_str(), meteo->getSensors().size(),
            meteo->getEnabled() ? F("Enabled") : F("Disabled"));
    }

    Serial.println("");
}

void CLIInformerClass::showMeteoStatus()
{
    for (auto *ctrl : Controllers.getControllers()) {
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

    Serial.println("");
}

void CLIInformerClass::showOneWire()
{
    unsigned i = 0;

    Serial.println("");

    for (auto iface : Interfaces.getInterfaces()) {
        if (iface->getType() != INT_TYPE_OW) {
            continue;
        }

        Serial.println(iface->getName() + ":\n");
        Serial.println(F("\tNum   Id"));
        Serial.println(F("\t---   ------------------"));

        auto ow = static_cast<IfOneWire *>(iface);
        std::vector<String> addrs;
        ow->findAddresses(addrs);

        i = 0;
        for (auto addr : addrs) {
            i++;
            Serial.printf("\t%-3d   %-18s\n", i, addr);
        }
        Serial.println("");
    }
}

void CLIInformerClass::showI2C()
{
    unsigned i = 0;

    Serial.println("");
    for (auto iface : Interfaces.getInterfaces()) {
        if (iface->getType() != INT_TYPE_I2C) {
            continue;
        }

        Serial.println(iface->getName() + ":\n");
        Serial.println(F("\tNum   Address   Hex"));
        Serial.println(F("\t---   -------   -----"));

        auto i2c = static_cast<IfI2C *>(iface);
        std::vector<unsigned> devs;
        i2c->findDevices(devs);

        i = 0;
        for (auto dev : devs) {
            i++;
            Serial.printf("\t%-3d   %-18s   %-5s\n", i, dev, String(dev, HEX));
        }
        Serial.println("");
    }
}

void CLIInformerClass::showTgBot()
{
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
    unsigned i = 1;
    for (auto *user : TgBot.getUsers()) {
        Serial.printf("\t%-3d   %-12s   %-9d   %-6s   %-5s\n", i, user->name.c_str(), user->chatId,
                    user->notify ? F("On") : F("Off"), user->admin ? F("True") : F("False"));
        i++;
    }
    Serial.println("");
}

CLIInformerClass CLIInformer;
