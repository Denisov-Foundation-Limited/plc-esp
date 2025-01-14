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

#include "utils/configs.hpp"
#include "boards/boards.hpp"
#include "core/ifaces/gpio.hpp"
#include "core/ifaces/i2c.hpp"
#include "core/ifaces/spi.hpp"
#include "core/ifaces/ow.hpp"
#include "core/ifaces/uart.hpp"
#include "net/tgbot.hpp"
#include "net/apiserver.hpp"
#include "controllers/meteo/meteo.hpp"
#include "controllers/meteo/sensors/msensor.hpp"
#include "controllers/meteo/sensors/ds18b20.hpp"
#include "controllers/ctrls.hpp"
#include "controllers/socket/socket.hpp"
#include "db/socketdb.hpp"

#include <LittleFS.h>
#include <SD.h>

/*********************************************************************/
/*                                                                   */
/*                          PUBLIC FUNCTIONS                         */
/*                                                                   */
/*********************************************************************/

bool ConfigsClass::begin()
{
    bool        isOk = false;

    _initInterfaces();
    Log.info(F("CFG"), "Interfaces initialized");

    /*if ((iface = Interfaces.getInterface(F("spi-sd"))) == nullptr) {
        Log.warning(F("CFG"), F("Interface SDcard SPI not found"));
    }
    auto *spiSD = static_cast<IfSPI *>(iface);

    SPI.end();
    SPI.begin(spiSD->getPin(SPI_PIN_SCK), spiSD->getPin(SPI_PIN_MISO), spiSD->getPin(SPI_PIN_MOSI));

    if (SD.begin(spiSD->getPin(SPI_PIN_SS), SPI, spiSD->getFrequency())) {
        Log.info(F("CFG"), F("SD card found. Reading files"));
        if (!SD.exists(CONFIGS_STARTUP_FILE))
        {
            _src = CFG_SRC_SD;
            return _initDevice();
        } else {
            return _readAll(CFG_SRC_SD);
        }
    }*/

    Log.warning(F("CFG"), F("SD card not found. Trying to read from flash memory"));

    _src = CFG_SRC_FLASH;

#ifdef ESP32
    isOk = LittleFS.begin(true);
#else
    isOk = LittleFS.begin();
#endif

    if (isOk) {
        Log.info(F("CFG"), F("Flash memory initialized"));
    } else {
        Log.error(F("CFG"), F("Failed to flash memory"));
    }

    if (!LittleFS.exists(CONFIGS_STARTUP_FILE))
    {
        return _initDevice();
    }

    return _readAll(CFG_SRC_FLASH);
}

bool ConfigsClass::writeAll()
{
    JsonDocument    doc;
    File            file;

    if (!_generateRunning(doc)) {
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
    return _printFile(CONFIGS_STARTUP_FILE);
}

bool ConfigsClass::showRunning()
{
    JsonDocument    doc;

    if (!_generateRunning(doc)) {
        return false;
    }

    serializeJsonPretty(doc, Serial);
    Serial.println("");
    doc.clear();

    return true;
}

/*********************************************************************/
/*                                                                   */
/*                          PRIVATE FUNCTIONS                        */
/*                                                                   */
/*********************************************************************/

void ConfigsClass::_initInterfaces()
{
}

bool ConfigsClass::_printFile(const String &name)
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

bool ConfigsClass::_initDevice()
{
    Log.info(F("CFG"), F("Configs not found. Init new device"));

    /* Wi-Fi setup */

    Wireless.setEnabled(ActiveBoard.wifi.enabled);
    Wireless.setHostname(ActiveBoard.wifi.hostname);
    Wireless.setCreds(ActiveBoard.wifi.ssid, ActiveBoard.wifi.passwd);

    /* TgBot setup */

    TgBot.setPollMode(fb::Poll::Long, 20000);

    return true;
}

bool ConfigsClass::_readAll(ConfigsSource src)
{
    JsonDocument    doc;
    File            file;

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
     * Wi-Fi configurations
     */

    auto jwifi = doc[F("wifi")];
    Wireless.setCreds(jwifi[F("ssid")], jwifi[F("passwd")]);
    Wireless.setHostname(jwifi[F("hostname")]);
    Wireless.setAP(jwifi[F("ap")]);
    Wireless.setEnabled(jwifi[F("enabled")]);

    /*
     * PLC configurations
     */

    auto jplc = doc[F("plc")];
    Plc.setFanEnabled(jplc[F("fan")]);
    Plc.setName(jplc[F("name")]);

    /*
     * Telegram configurations
     */

    auto jtgbot = doc[F("tgbot")];
    TgBot.setToken(jtgbot[F("token")]);
    fb::Poll poll;
    if (jtgbot[F("mode")] == "sync") {
        poll = fb::Poll::Sync;
    } else if (jtgbot[F("mode")] == "async") {
        poll = fb::Poll::Async;
    } else if (jtgbot[F("mode")] == "long") {
        poll = fb::Poll::Long;
    }
    TgBot.setPollMode(poll, jtgbot[F("period")]);
    JsonArray jusers = jtgbot[F("users")];
    unsigned k = 0;
    for (auto usr : jusers) {
        TgUser  user;
        memset(&user, 0x0, sizeof(TgUser));
        user.name = usr[F("name")].as<String>();
        user.chatId = usr[F("id")];
        user.notify = usr[F("notify")];
        user.admin = usr[F("admin")];
        user.enabled = true;
        user.level = TG_MENU_MAIN;
        TgBot.setUser(k, &user);
        k++;
    }
    TgBot.setEnabled(jtgbot[F("enabled")]);

    /*
     * Socket configurations
     */

    auto jctrls = doc[F("controllers")];
    JsonArray jsocks = jctrls["socket"];
    for (size_t i = 0; i < jsocks.size(); i++) {
        Socket sock;
        memset(&sock, 0x0, sizeof(Socket));
        sock.id = jsocks[i][F("id")].as<unsigned>();
        sock.name = jsocks[i][F("name")].as<String>();
        sock.enabled = true;
        Gpio.getPinById(jsocks[i][F("relay")].as<unsigned>(), &sock.relay);
        Gpio.getPinById(jsocks[i][F("button")].as<unsigned>(), &sock.button);
        SocketCtrl.setSocket(jsocks[i][F("id")].as<unsigned>() - 1, &sock);
    }

    doc.clear();
    return true;
}

bool ConfigsClass::loadStates()
{
    
    return true;
}

bool ConfigsClass::_generateRunning(JsonDocument &doc)
{
    /*
     * PLC general configurations
     */

    auto jplc = doc[F("plc")];
    jplc[F("name")] = Plc.getName();
    jplc[F("fan")] = Plc.getFanEnabled();

    /*
     * Network configurations
     */

    auto jwifi = doc[F("wifi")];
    jwifi[F("enabled")] = Wireless.getEnabled();
    jwifi[F("hostname")] = Wireless.getHostname();
    jwifi[F("ssid")] = Wireless.getSSID();
    jwifi[F("passwd")] = Wireless.getPasswd();
    jwifi[F("ap")] = Wireless.getAP();

    /*
     * GSM modem configurations
     */

    auto jgsm = doc[F("gsm")];
    jgsm[F("enabled")] = GsmModem.getEnabled();

    /*
     * Telegram Bot
     */

    auto jtgbot = doc[F("tgbot")];
    jtgbot[F("enabled")] = TgBot.getEnabled();
    jtgbot[F("token")] = TgBot.getToken();
    jtgbot[F("period")] = TgBot.getPollPeriod();
    switch (TgBot.getPollMode()) {
        case fb::Poll::Async:
            jtgbot[F("mode")] = F("async");
            break;
        case fb::Poll::Sync:
            jtgbot[F("mode")] = F("sync");
            break;
        case fb::Poll::Long:
            jtgbot[F("mode")] = F("long");
            break;
    }
    auto jusers = jtgbot[F("users")];

    unsigned k = 0;
    std::vector<TgUser *> users;
    TgBot.getEnabledUsers(users);

    for (auto *usr : users) {
        jusers[k][F("name")] = usr->name;
        jusers[k][F("id")] = usr->chatId;
        jusers[k][F("notify")] = usr->notify;
        jusers[k][F("admin")] = usr->admin;
        k++;
    }

    /*
     * Socket controller
     */

    auto jctrls = doc[F("controllers")];
    auto jsocks = jctrls[F("socket")];
    
    std::vector<Socket *> socks;
    SocketCtrl.getEnabledSockets(socks);

    for (size_t i = 0; i < socks.size(); i++) {
        jsocks[i][F("id")] = socks[i]->id;
        jsocks[i][F("name")] = socks[i]->name;
        (socks[i]->button == nullptr) ? jsocks[i][F("button")] = 0 : jsocks[i][F("button")] = socks[i]->button->id;
        (socks[i]->relay == nullptr) ? jsocks[i][F("relay")] = 0 : jsocks[i][F("relay")] = socks[i]->relay->id;
    }

    return true;
}

ConfigsSource ConfigsClass::getSource() const
{
    return _src;
}

ConfigsClass Configs;
