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

#include "utils/configs.hpp"
#include "boards/boards.hpp"
#include "core/ifaces/gpio.hpp"
#include "core/ifaces/i2c.hpp"
#include "core/ifaces/spi.hpp"
#include "core/ifaces/ow.hpp"
#include "core/ifaces/uart.hpp"

#include <LittleFS.h>
#include <SD.h>

/*********************************************************************/
/*                                                                   */
/*                          PRIVATE FUNCTIONS                        */
/*                                                                   */
/*********************************************************************/

bool ConfigsClass::printFile(const String &name)
{
    File    file;

    if (_src == CFG_SRC_SD) {
        file = SD.open(name, "r");
    } else {
        file = LittleFS.open(name, "r");
    }
    if (!file) {
        return false;
    }
    while (file.available()) {
        Serial.print(file.readString());
    }
    Serial.println("");
    file.close();  

    return true;
}

bool ConfigsClass::initDevice()
{
    Log.info(LOG_MOD_CFG, F("Configs not found. Init new device"));

    for (auto& e : extenders) {
        Extenders.addExtender(new Extender(static_cast<ExtenderId>(e.id), e.addr));
    }

    /* GPIO Interface */

    int i = 1;
    for (auto& p : inputs) {
        Interfaces.addInterface(new GPIOIface("in-0/" + String(i), p, GPIO_MOD_INPUT, GPIO_PULL_NONE, EXT_NOT_USED));
        i++;
    }

    i = 1;
    for (auto& p : extInputs) {
        if (Extenders.getById(static_cast<ExtenderId>(p.extId)) != nullptr) {
            Interfaces.addInterface(new GPIOIface("in-" + String(p.extId) + "/" + String(i), p.pin, GPIO_MOD_INPUT, GPIO_PULL_NONE, static_cast<ExtenderId>(p.extId)));
        }
        i++;
    }

    i = 1;
    for (auto& p : relays) {
        auto pin = new GPIOIface("rly-0/" + String(i), p, GPIO_MOD_OUTPUT, GPIO_PULL_NONE, EXT_NOT_USED);
        pin->write(false);
        Interfaces.addInterface(pin);
        i++;
    }

    i = 1;
    for (auto& p : extRelays) {
        if (Extenders.getById(static_cast<ExtenderId>(p.extId)) != nullptr) {
            auto pin = new GPIOIface("rly-" + String(p.extId) + "/" + String(i), p.pin, GPIO_MOD_OUTPUT, GPIO_PULL_NONE, static_cast<ExtenderId>(p.extId));
            pin->write(false);
            Interfaces.addInterface(pin);
        }
        i++;
    }

    /* Sensors Interface */

    i = 1;
    for (auto& p : sensors) {
        Interfaces.addInterface(new GPIOIface("sens-0/" + String(i), p, GPIO_MOD_INPUT, GPIO_PULL_NONE, EXT_NOT_USED));
        i++;
    }

    /* OneWire Interface */

    i = 1;
    for (auto& p : ows) {
        Interfaces.addInterface(new OneWireIface("ow-" + String(i), p));
        i++;
    }

    /* I2C Interface */

    Interfaces.addInterface(new I2CIface(F("i2c"), I2C_SDA_PIN, I2C_SCL_PIN));

    /* SPI Interface */

    Interfaces.addInterface(new SPIface(F("spi"), SPI_MOSI_PIN, SPI_MISO_PIN, SPI_SCK_PIN, SPI_SS_PIN, 0));

    /* UART Interface */

    Interfaces.addInterface(new UARTIface(F("uart-gsm"), GSM_RX_PIN, GSM_TX_PIN, GSM_RST_PIN, GSM_MODEM_RATE));
    Interfaces.addInterface(new UARTIface(F("uart-485"), RS485_RX_PIN, RS485_TX_PIN, RS485_IO_PIN, RS485_TRANSFER_RATE));

    /* GSM Modem setup */

    Interface *iface = nullptr;

    if ((iface = Interfaces.getInterface("uart-gsm")) == nullptr) {
        Log.warning(LOG_MOD_CFG, F("Interface uart-gsm not found"));
    }
    GsmModem.setUart(static_cast<UARTIface *>(iface));

    return true;
}

bool ConfigsClass::readAll(ConfigsSource src)
{
    JsonDocument    doc;
    File            file;
    Interface       *pin = nullptr;

    _src = src;

    /*
     * Loading configs from file
     */

    if (src == CFG_SRC_SD) {
        file = SD.open(CONFIGS_STARTUP_FILE, "r");
    } else {
        file = LittleFS.open(CONFIGS_STARTUP_FILE, "r");
    }
    deserializeJson(doc, file);
    file.close();

    /*
     * Extenders configurations
     */

    JsonArray exts = doc["extenders"];
    for (uint8_t i = 0; i < exts.size(); i++) {
        Extenders.addExtender(new Extender(static_cast<ExtenderId>(exts[i]["id"]), exts[i]["addr"]));
    }

    /*
     * GPIO configurations
     */

    JsonArray jifaces = doc["interfaces"];
    for (uint8_t i = 0; i < jifaces.size(); i++) {
        if (jifaces[i]["type"] == "gpio") {
            GpioMode    type = GPIO_MOD_INPUT;
            GpioPull    pull = GPIO_PULL_NONE;

            if (jifaces[i]["mode"] == "input") {
                type = GPIO_MOD_INPUT;
            } else if (jifaces[i]["mode"] == "output") {
                type = GPIO_MOD_OUTPUT;
            } else {
                Log.error(LOG_MOD_CFG, String(F("GPIO mode not found: ")) + jifaces[i]["mode"].as<String>());
                doc.clear();
                return false;
            }

            if (jifaces[i]["pull"] == "none") {
                pull = GPIO_PULL_NONE;
            } else if (jifaces[i]["pull"] == "up") {
                pull = GPIO_PULL_UP;
            } else if (jifaces[i]["pull"] == "down") {
                pull = GPIO_PULL_DOWN;
            } else {
                Log.error(LOG_MOD_CFG, String(F("GPIO pull not found: ")) + jifaces[i]["pull"].as<String>());
                doc.clear();
                return false;
            }

            Interfaces.addInterface(new GPIOIface(jifaces[i]["name"], jifaces[i]["pin"], type, pull, jifaces[i]["ext"]));
        } else if (jifaces[i]["type"] == "ow") {
            Interfaces.addInterface(new OneWireIface(jifaces[i]["name"], jifaces[i]["pin"]));
        } else if (jifaces[i]["type"] == "spi") {
            Interfaces.addInterface(new SPIface(jifaces[i]["name"], jifaces[i]["miso"], jifaces[i]["mosi"],
                                                jifaces[i]["sck"], jifaces[i]["ss"], jifaces[i]["freq"]));
        } else if (jifaces[i]["type"] == "i2c") {
            Interfaces.addInterface(new I2CIface(jifaces[i]["name"], jifaces[i]["sda"], jifaces[i]["scl"]));
        } else if (jifaces[i]["type"] == "uart") {
            Interfaces.addInterface(new UARTIface(jifaces[i]["name"], jifaces[i]["rx"], jifaces[i]["tx"], jifaces[i]["ctrl"], jifaces[i]["rate"]));
        } else {
            Log.error(LOG_MOD_CFG, String(F("Interface type unknown: ")) + jifaces[i]["type"].as<String>());
            doc.clear();
            return false;
        }
    }

    /*
     * PLC general configurations
     */

    Plc.setName(doc["plc"]["name"]);
    if ((pin = Interfaces.getInterface(doc["plc"]["iface"]["alarm"])) == nullptr) {
        Log.warning(LOG_MOD_CFG, F("PLC gpio Alarm not found"));
    }
    Plc.setPin(PLC_GPIO_ALARM_LED, static_cast<GPIOIface *>(pin));
    if ((pin = Interfaces.getInterface(doc["plc"]["iface"]["buzzer"])) == nullptr) {
        Log.warning(LOG_MOD_CFG, F("PLC gpio Buzzer not found"));
    }
    Plc.setPin(PLC_GPIO_BUZZER, static_cast<GPIOIface *>(pin));

    /*
     * Wi-Fi configurations
     */

    Wireless.setCreds(doc["wifi"]["ssid"], doc["wifi"]["passwd"]);
    Wireless.setAP(doc["wifi"]["ap"]);
    if ((pin = Interfaces.getInterface(doc["wifi"]["iface"]["status"])) == nullptr) {
        Log.warning(LOG_MOD_CFG, F("PLC interface for WiFi status led not found"));
     }
    Wireless.setStatusLed(static_cast<GPIOIface *>(pin));
    Wireless.setEnabled(doc["wifi"]["enabled"]);

    /*
     * GSM modem configurations
     */

    if ((pin = Interfaces.getInterface(doc["gsm"]["iface"])) == nullptr) {
        Log.warning(LOG_MOD_CFG, F("Interface uart-gsm not found"));
    }
    GsmModem.setUart(static_cast<UARTIface *>(pin));
    GsmModem.setEnabled(doc["gsm"]["enabled"]);
    GsmModem.begin();

    doc.clear();
    return true;
}

bool ConfigsClass::generateRunning(JsonDocument &doc)
{
    /*
     * PLC general configurations
     */

    doc["plc"]["name"] = Plc.getName();
    doc["plc"]["iface"]["alarm"] = (Plc.getPin(PLC_GPIO_ALARM_LED) == nullptr) ? "" : Plc.getPin(PLC_GPIO_ALARM_LED)->getName();
    doc["plc"]["iface"]["buzzer"] = (Plc.getPin(PLC_GPIO_BUZZER) == nullptr) ? "" : Plc.getPin(PLC_GPIO_BUZZER)->getName();

    /*
     * Wi-Fi configurations
     */

    doc["wifi"]["ssid"] = Wireless.getSSID();
    doc["wifi"]["passwd"] = Wireless.getPasswd();
    doc["wifi"]["ap"] = Wireless.getAP();
    doc["wifi"]["gpio"]["status"] = (Wireless.getStatusLed() == nullptr) ? "" : Wireless.getStatusLed()->getName();
    doc["wifi"]["enabled"] = Wireless.getEnabled();

    /*
     * GSM modem configurations
     */

    doc["gsm"]["iface"] = (GsmModem.getUart() == nullptr) ? "" : GsmModem.getUart()->getName();
    doc["gsm"]["enabled"] = GsmModem.getEnabled();

    /*
     * Extenders configurations
     */

    for (uint8_t i = 0; i < Extenders.getExtenders().size(); i++) {
        auto e = Extenders.getExtenders()[i];
        doc["extenders"][i]["id"] = e->getID();
        doc["extenders"][i]["addr"] = e->getAddr();
    }

    /*
     * GPIO configurations
     */

    for (uint8_t i = 0; i < Interfaces.getInterfaces().size(); i++) {
        auto p = Interfaces.getInterfaces()[i];
        doc["interfaces"][i]["name"] = p->getName();

        switch (p->getType()) {
            case INT_TYPE_GPIO:
                doc["interfaces"][i]["type"] = "gpio";
                break;

            case INT_TYPE_OW:
                doc["interfaces"][i]["type"] = "ow";
                break;

            case INT_TYPE_I2C:
                doc["interfaces"][i]["type"] = "i2c";
                break;

            case INT_TYPE_SPI:
                doc["interfaces"][i]["type"] = "spi";
                break;
        }

        if (p->getType() == INT_TYPE_GPIO) {
            auto gpio = static_cast<GPIOIface *>(p);
            doc["interfaces"][i]["pin"] = gpio->getPin();
            switch (gpio->getMode()) {
                case GPIO_MOD_INPUT:
                    doc["interfaces"][i]["mode"] = "input";
                    break;

                case GPIO_MOD_OUTPUT:
                    doc["interfaces"][i]["mode"] = "output";
                    break;
            }
            switch (gpio->getPull()) {
                case GPIO_PULL_NONE:
                    doc["interfaces"][i]["pull"] = "none";
                    break;

                case GPIO_PULL_UP:
                    doc["interfaces"][i]["pull"] = "up";
                    break;

                case GPIO_PULL_DOWN:
                    doc["interfaces"][i]["pull"] = "down";
                    break;
            }
            doc["interfaces"][i]["ext"] = gpio->getExtId();
        } else if (p->getType() == INT_TYPE_SPI) {
            auto spi = static_cast<SPIface *>(p);
            doc["interfaces"][i]["miso"] = spi->getPin(SPI_PIN_MISO);
            doc["interfaces"][i]["mosi"] = spi->getPin(SPI_PIN_MOSI);
            doc["interfaces"][i]["sck"] = spi->getPin(SPI_PIN_SCK);
            doc["interfaces"][i]["ss"] = spi->getPin(SPI_PIN_SS);
            doc["interfaces"][i]["freq"] = spi->getFrequency();
        } else if (p->getType() == INT_TYPE_OW) {
            auto ow = static_cast<OneWireIface *>(p);
            doc["interfaces"][i]["pin"] = ow->getPin();
        } else if (p->getType() == INT_TYPE_I2C) {
            auto i2c = static_cast<I2CIface *>(p);
            doc["interfaces"][i]["miso"] = i2c->getPin(I2C_PIN_SDA);
            doc["interfaces"][i]["mosi"] = i2c->getPin(I2C_PIN_SCL);
        } else if (p->getType() == INT_TYPE_UART) {
            auto uart = static_cast<UARTIface *>(p);
            doc["interfaces"][i]["rx"] = uart->getPin(UART_PIN_RX);
            doc["interfaces"][i]["tx"] = uart->getPin(UART_PIN_TX);
            doc["interfaces"][i]["ctrl"] = uart->getPin(UART_PIN_CTRL);
            doc["interfaces"][i]["rate"] = uart->getRate();
        }
    }

    return true;
}

/*********************************************************************/
/*                                                                   */
/*                          PUBLIC FUNCTIONS                         */
/*                                                                   */
/*********************************************************************/

bool ConfigsClass::begin()
{
    bool isOk = false;

    if (SD.begin(SPI_SS_PIN)) {
        Log.info(LOG_MOD_CFG, F("SD card found. Reading files"));
        if (!SD.exists(CONFIGS_STARTUP_FILE))
        {
            _src = CFG_SRC_SD;
            return initDevice();
        } else {
            return readAll(CFG_SRC_SD);
        }
    }

    Log.warning(LOG_MOD_CFG, F("SD card not found. Trying to read from flash memory"));

#ifdef ESP32
    isOk = LittleFS.begin(true);
#else
    isOk = LittleFS.begin();
#endif

    if (isOk) {
        Log.info(LOG_MOD_CFG, F("Flash memory initialized"));
    } else {
        Log.error(LOG_MOD_CFG, F("Failed to flash memory"));
    }

    if (!LittleFS.exists(CONFIGS_STARTUP_FILE))
    {
        _src = CFG_SRC_FLASH;
        return initDevice();
    }

    return readAll(CFG_SRC_FLASH);
}

bool ConfigsClass::writeAll()
{
    JsonDocument    doc;
    File            file;

    if (!generateRunning(doc)) {
        return false;
    }

    /*
     * Save configs to file
     */

    if (_src == CFG_SRC_SD) {
        file = SD.open(CONFIGS_STARTUP_FILE, "w");
    } else {
        file = LittleFS.open(CONFIGS_STARTUP_FILE, "w");
    }
    serializeJsonPretty(doc, file);
    file.close();
    doc.clear();

    return true;
}

bool ConfigsClass::eraseAll()
{
    if (_src == CFG_SRC_SD) {
        return SD.remove(CONFIGS_STARTUP_FILE);
    }

    return LittleFS.remove(CONFIGS_STARTUP_FILE);;
}

bool ConfigsClass::showStartup()
{
    return printFile(CONFIGS_STARTUP_FILE);
}

bool ConfigsClass::showRunning()
{
    JsonDocument    doc;

    if (!generateRunning(doc)) {
        return false;
    }

    serializeJsonPretty(doc, Serial);
    Serial.println("");
    doc.clear();

    return true;
}

ConfigsClass Configs;
