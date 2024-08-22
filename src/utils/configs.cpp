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

#include <LittleFS.h>
#include <SD.h>

/*********************************************************************/
/*                                                                   */
/*                          PRIVATE FUNCTIONS                        */
/*                                                                   */
/*********************************************************************/

bool Configs::printFile(const String &name)
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

bool Configs::initDevice()
{
    _log->info(LOG_MOD_CFG, F("Configs not found. Init new device"));

    for (auto& e : extenders) {
        _ext->addExtender(new Extender(static_cast<ExtenderId>(e.id), e.addr));
    }

    /* CPU GPIO */

    int i = 1;
    for (auto& p : inputs) {
        _gpio->addGpio(new GpioPin(_log, _ext, "in-0/" + String(i), p, GPIO_TYPE_INPUT, GPIO_PULL_NONE, EXT_NOT_USED));
        i++;
    }

    i = 1;
    for (auto& p : relays) {
        auto pin = new GpioPin(_log, _ext, "rly-0/" + String(i), p, GPIO_TYPE_OUTPUT, GPIO_PULL_NONE, EXT_NOT_USED);
        pin->write(false);
        _gpio->addGpio(pin);
        i++;
    }

    /* Extenders GPIO */

    i = 1;
    for (auto& p : extInputs) {
        if (_ext->getById(static_cast<ExtenderId>(p.extId)) != nullptr) {
            _gpio->addGpio(new GpioPin(_log, _ext, "in-" + String(p.extId) + "/" + String(i), p.pin, GPIO_TYPE_INPUT, GPIO_PULL_NONE, static_cast<ExtenderId>(p.extId)));
        }
        i++;
    }

    i = 1;
    for (auto& p : extRelays) {
        if (_ext->getById(static_cast<ExtenderId>(p.extId)) != nullptr) {
            auto pin = new GpioPin(_log, _ext, "rly-" + String(p.extId) + "/" + String(i), p.pin, GPIO_TYPE_INPUT, GPIO_PULL_NONE, static_cast<ExtenderId>(p.extId));
            pin->write(false);
            _gpio->addGpio(pin);
        }
        i++;
    }

    /* I2C GPIO */

    _gpio->addGpio(new GpioPin(_log, _ext, F("i2c-sda"), I2C_SDA_PIN, GPIO_TYPE_OUTPUT, GPIO_PULL_NONE, EXT_NOT_USED));
    _gpio->addGpio(new GpioPin(_log, _ext, F("i2c-scl"), I2C_SDA_PIN, GPIO_TYPE_OUTPUT, GPIO_PULL_NONE, EXT_NOT_USED));

    /* SPI GPIO */

    _gpio->addGpio(new GpioPin(_log, _ext, F("spi-mosi"), SPI_MOSI_PIN, GPIO_TYPE_OUTPUT, GPIO_PULL_NONE, EXT_NOT_USED));
    _gpio->addGpio(new GpioPin(_log, _ext, F("spi-miso"), SPI_MISO_PIN, GPIO_TYPE_OUTPUT, GPIO_PULL_NONE, EXT_NOT_USED));
    _gpio->addGpio(new GpioPin(_log, _ext, F("spi-sck"), SPI_SCK_PIN, GPIO_TYPE_OUTPUT, GPIO_PULL_NONE, EXT_NOT_USED));
    _gpio->addGpio(new GpioPin(_log, _ext, F("spi-ss"), SPI_SS_PIN, GPIO_TYPE_OUTPUT, GPIO_PULL_NONE, EXT_NOT_USED));

    /* OneWire GPIO */

    i = 1;
    for (auto& p : ows) {
        auto pin = new GpioPin(_log, _ext, "ow-0/" + String(i), p, GPIO_TYPE_INPUT, GPIO_PULL_NONE, EXT_NOT_USED);
        pin->write(false);
        _gpio->addGpio(pin);
        i++;
    }

    /* RS485 GPIO */

    i = 1;
    for (auto& p : rs485pins) {
        _gpio->addGpio(new GpioPin(_log, _ext, "rs485-0/" + String(i) + "-rx", p.rx, GPIO_TYPE_OUTPUT, GPIO_PULL_NONE, EXT_NOT_USED));
        _gpio->addGpio(new GpioPin(_log, _ext, "rs485-0/" + String(i) + "-tx", p.rx, GPIO_TYPE_OUTPUT, GPIO_PULL_NONE, EXT_NOT_USED));
        _gpio->addGpio(new GpioPin(_log, _ext, "rs485-0/" + String(i) + "-io", p.rx, GPIO_TYPE_OUTPUT, GPIO_PULL_NONE, EXT_NOT_USED));
        i++;
    }

    /* GSM Modem GPIO */

    _gpio->addGpio(new GpioPin(_log, _ext, F("gsm-rx"), GSM_RX_PIN, GPIO_TYPE_OUTPUT, GPIO_PULL_NONE, EXT_NOT_USED));
    _gpio->addGpio(new GpioPin(_log, _ext, F("gsm-tx"), GSM_TX_PIN, GPIO_TYPE_OUTPUT, GPIO_PULL_NONE, EXT_NOT_USED));
    _gpio->addGpio(new GpioPin(_log, _ext, F("gsm-rst"), GSM_RST_PIN, GPIO_TYPE_OUTPUT, GPIO_PULL_NONE, EXT_NOT_USED));

    /* Sensors GPIO */

    i = 1;
    for (auto& p : sensors) {
        _gpio->addGpio(new GpioPin(_log, _ext, "sens-0/" + String(i), p, GPIO_TYPE_INPUT, GPIO_PULL_NONE, EXT_NOT_USED));
        i++;
    }

    /* GSM Modem setup */

    GpioPin *pin = nullptr;

    if ((pin = _gpio->getPin("gsm-rx")) == nullptr) {
        _log->warning(LOG_MOD_CFG, F("Gpio gsm-rx not found"));
    }
    _modem->setGpio(GSM_GPIO_RX, pin);
    if ((pin = _gpio->getPin("gsm-tx")) == nullptr) {
        _log->warning(LOG_MOD_CFG, F("Gpio gsm-tx not found"));
    }
    _modem->setGpio(GSM_GPIO_TX, pin);
    if ((pin = _gpio->getPin("gsm-rst")) == nullptr) {
        _log->warning(LOG_MOD_CFG, F("Gpio gsm-rst not found"));
    }
    _modem->setGpio(GSM_GPIO_RST, pin);
    _modem->setRate(GSM_MODEM_RATE);

    return true;
}

bool Configs::readAll(ConfigsSource src)
{
    JsonDocument    doc;
    File            file;
    GpioPin         *pin = nullptr;

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
        _ext->addExtender(new Extender(static_cast<ExtenderId>(exts[i]["id"]), exts[i]["addr"]));
    }

    /*
     * GPIO configurations
     */

    JsonArray gpios = doc["gpio"];
    for (uint8_t i = 0; i < gpios.size(); i++) {
        GpioType    type = GPIO_TYPE_INPUT;
        GpioPull    pull = GPIO_PULL_NONE;

        if (gpios[i]["type"] == "input") {
            type = GPIO_TYPE_INPUT;
        } else if (gpios[i]["type"] == "output") {
            type = GPIO_TYPE_OUTPUT;
        } else {
            _log->error(LOG_MOD_CFG, String(F("GPIO type not found: ")) + gpios[i]["type"].as<String>());
            doc.clear();
            return false;
        }

        if (gpios[i]["pull"] == "none") {
            pull = GPIO_PULL_NONE;
        } else if (gpios[i]["pull"] == "up") {
            pull = GPIO_PULL_UP;
        } else if (gpios[i]["pull"] == "down") {
            pull = GPIO_PULL_DOWN;
        } else {
            _log->error(LOG_MOD_CFG, String(F("GPIO pull not found: ")) + gpios[i]["pull"].as<String>());
            doc.clear();
            return false;
        }

        _gpio->addGpio(new GpioPin(_log, _ext, gpios[i]["name"], gpios[i]["pin"], type, pull, gpios[i]["ext"]));
    }

    /*
     * PLC general configurations
     */

    _plc->setName(doc["plc"]["name"]);
    if ((pin = _gpio->getPin(doc["plc"]["gpio"]["alarm"])) == nullptr) {
        _log->warning(LOG_MOD_CFG, F("PLC gpio Alarm not found"));
    }
    _plc->setPin(PLC_GPIO_ALARM_LED, pin);
    if ((pin = _gpio->getPin(doc["plc"]["gpio"]["buzzer"])) == nullptr) {
        _log->warning(LOG_MOD_CFG, F("PLC gpio Buzzer not found"));
    }
    _plc->setPin(PLC_GPIO_BUZZER, pin);

    /*
     * Wi-Fi configurations
     */

    _wifi->setCreds(doc["wifi"]["ssid"], doc["wifi"]["passwd"]);
    _wifi->setAP(doc["wifi"]["ap"]);
    if ((pin = _gpio->getPin(doc["wifi"]["gpio"]["status"])) == nullptr) {
        _log->warning(LOG_MOD_CFG, F("PLC gpio WiFi status led not found"));
     }
    _wifi->setStatusLed(pin);
    _wifi->setEnabled(doc["wifi"]["enabled"]);

    /*
     * GSM modem configurations
     */

    if ((pin = _gpio->getPin(doc["gsm"]["gpio"]["rx"])) == nullptr) {
        _log->warning(LOG_MOD_CFG, F("Gpio GSM rx not found"));
    }
    _modem->setGpio(GSM_GPIO_RX, pin);
    if ((pin = _gpio->getPin(doc["gsm"]["gpio"]["tx"])) == nullptr) {
        _log->warning(LOG_MOD_CFG, F("Gpio GSM tx not found"));
    }
    _modem->setGpio(GSM_GPIO_TX, pin);
    if ((pin = _gpio->getPin(doc["gsm"]["gpio"]["rst"])) == nullptr) {
        _log->warning(LOG_MOD_CFG, F("Gpio GSM rst not found"));
     }
    _modem->setGpio(GSM_GPIO_RST, pin);
    _modem->setEnabled(doc["gsm"]["enabled"]);
    _modem->setRate(doc["gsm"]["rate"]);
    _modem->begin();

    doc.clear();
    return true;
}

bool Configs::generateRunning(JsonDocument &doc)
{
    /*
     * PLC general configurations
     */

    doc["plc"]["name"] = _plc->getName();
    doc["plc"]["gpio"]["alarm"] = (_plc->getPin(PLC_GPIO_ALARM_LED) == nullptr) ? "" : _plc->getPin(PLC_GPIO_ALARM_LED)->getName();
    doc["plc"]["gpio"]["buzzer"] = (_plc->getPin(PLC_GPIO_BUZZER) == nullptr) ? "" : _plc->getPin(PLC_GPIO_BUZZER)->getName();

    /*
     * Wi-Fi configurations
     */

    doc["wifi"]["ssid"] = _wifi->getSSID();
    doc["wifi"]["passwd"] = _wifi->getPasswd();
    doc["wifi"]["ap"] = _wifi->getAP();
    doc["wifi"]["gpio"]["status"] = (_wifi->getStatusLed() == nullptr) ? "" : _wifi->getStatusLed()->getName();
    doc["wifi"]["enabled"] = _wifi->getEnabled();

    /*
     * GSM modem configurations
     */

    doc["gsm"]["gpio"]["rx"] = (_modem->getGpio(GSM_GPIO_RX) == nullptr) ? "" : _modem->getGpio(GSM_GPIO_RX)->getName();
    doc["gsm"]["gpio"]["tx"] = (_modem->getGpio(GSM_GPIO_TX) == nullptr) ? "" : _modem->getGpio(GSM_GPIO_TX)->getName();
    doc["gsm"]["gpio"]["rst"] = (_modem->getGpio(GSM_GPIO_RST) == nullptr) ? "" : _modem->getGpio(GSM_GPIO_RST)->getName();
    doc["gsm"]["enabled"] = _modem->getEnabled();
    doc["gsm"]["rate"] = _modem->getRate();

    /*
     * Extenders configurations
     */

    for (uint8_t i = 0; i < _ext->getExtenders().size(); i++) {
        auto e = _ext->getExtenders()[i];
        doc["extenders"][i]["id"] = e->getID();
        doc["extenders"][i]["addr"] = e->getAddr();
    }

    /*
     * GPIO configurations
     */

    for (uint8_t i = 0; i < _gpio->getPins().size(); i++) {
        auto p = _gpio->getPins()[i];
        doc["gpio"][i]["name"] = p->getName();
        doc["gpio"][i]["pin"] = p->getPin();

        switch (p->getType()) {
            case GPIO_TYPE_INPUT:
                doc["gpio"][i]["type"] = "input";
                break;

            case GPIO_TYPE_OUTPUT:
                doc["gpio"][i]["type"] = "output";
                break;
        }

        switch (p->getPull()) {
            case GPIO_PULL_NONE:
                doc["gpio"][i]["pull"] = "none";
                break;

            case GPIO_PULL_UP:
                doc["gpio"][i]["pull"] = "up";
                break;

            case GPIO_PULL_DOWN:
                doc["gpio"][i]["pull"] = "down";
                break;
        }

        doc["gpio"][i]["ext"] = p->getExtId();
    }

    return true;
}

/*********************************************************************/
/*                                                                   */
/*                          PUBLIC FUNCTIONS                         */
/*                                                                   */
/*********************************************************************/

Configs::Configs(Logger *log, GsmModem *modem, Extenders *ext, Gpio *gpio, Wireless *wifi, Plc *plc)
{
    _log = log;
    _modem = modem;
    _ext = ext;
    _gpio = gpio;
    _wifi = wifi;
    _plc = plc;
}

bool Configs::begin()
{
    bool isOk = false;

    if (SD.begin(SPI_SS_PIN)) {
        _log->info(LOG_MOD_CFG, F("SD card found. Reading files"));
        if (!SD.exists(CONFIGS_STARTUP_FILE))
        {
            _src = CFG_SRC_SD;
            return initDevice();
        } else {
            return readAll(CFG_SRC_SD);
        }
    }

    _log->warning(LOG_MOD_CFG, F("SD card not found. Trying to read from flash memory"));

#ifdef ESP32
    isOk = LittleFS.begin(true);
#else
    isOk = LittleFS.begin();
#endif

    if (isOk) {
        _log->info(LOG_MOD_CFG, F("Flash memory initialized"));
    } else {
        _log->error(LOG_MOD_CFG, F("Failed to flash memory"));
    }

    if (!LittleFS.exists(CONFIGS_STARTUP_FILE))
    {
        _src = CFG_SRC_FLASH;
        return initDevice();
    }

    return readAll(CFG_SRC_FLASH);
}

bool Configs::writeAll()
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

bool Configs::eraseAll()
{
    return true;
}

bool Configs::showStartup()
{
    return printFile(CONFIGS_STARTUP_FILE);
}

bool Configs::showRunning()
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
