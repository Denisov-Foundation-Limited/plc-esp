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
#include "controllers/meteo.hpp"
#include "controllers/ctrls.hpp"
#include "controllers/socket.hpp"
#include "controllers/climate.hpp"
#include "controllers/security.hpp"
#include "db/socketdb.hpp"
#include "core/clock.hpp"
#include "controllers/tank.hpp"

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

    auto jclock = doc[F("clock")];
    Clock.setUTC(jclock[F("utc")]);

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
     * Controllers configurations
     */

    auto jctrls = doc[F("controllers")];

    /*
     * Socket configurations
     */
    
    auto jsock = jctrls["socket"];
    SocketCtrl.setEnabled(jsock["enabled"].as<bool>());

    JsonArray jsocks = jsock["sockets"];
    for (size_t i = 0; i < jsocks.size(); i++) {
        Socket sock;
        memset(&sock, 0x0, sizeof(Socket));
        sock.id = jsocks[i][F("id")].as<unsigned>();
        sock.name = jsocks[i][F("name")].as<String>();
        sock.enabled = true;
        Gpio.getPinById(jsocks[i][F("relay")].as<unsigned>(), &sock.relay);
        Gpio.getPinById(jsocks[i][F("button")].as<unsigned>(), &sock.button);
        Gpio.getPinById(jsocks[i][F("led")].as<unsigned>(), &sock.led);
        SocketCtrl.setSocket(jsocks[i][F("id")].as<unsigned>() - 1, &sock);
    }

    /*
     * Meteo configurations
     */

    auto jmeteo = jctrls["meteo"];
    MeteoCtrl.setEnabled(jmeteo["enabled"].as<bool>());

    JsonArray jsensors = jmeteo["sensors"];
    for (size_t i = 0; i < jsensors.size(); i++) {
        MeteoSensor sensor;

        memset(&sensor, 0x0, sizeof(MeteoSensor));
        sensor.id = jsensors[i][F("id")].as<unsigned>();
        sensor.name = jsensors[i][F("name")].as<String>();
        sensor.enabled = true;

        auto mtype = jsensors[i][F("type")].as<String>();
        if (mtype == "ds18b20") {
            sensor.type = METEO_SENSOR_DS18B20;
            sensor.addr = strtoull(jsensors[i][F("addr")].as<String>().c_str(), NULL, 16);
        } else if (mtype == "dht22") {
            sensor.type = METEO_SENSOR_DHT22;
        } else if (mtype == "am2302") {
            sensor.type = METEO_SENSOR_AM2302;
        } else if (mtype == "bme280") {
            sensor.type = METEO_SENSOR_BME280;
        }

        Gpio.getPinById(jsensors[i][F("pin")].as<unsigned>(), &sensor.pin);
        MeteoCtrl.setSensor(sensor.id - 1, &sensor);
    }

    /*
     * Climate controller
     */

    auto jclimate = jctrls["climate"];
    ClimateCtrl.setEnabled(jclimate["enabled"].as<bool>());
    JsonArray jzones = jclimate["zones"];

    for (size_t i = 0; i < jzones.size(); i++) {
        ClimateZone zone;

        memset(&zone, 0x0, sizeof(ClimateZone));
        zone.id = jzones[i][F("id")].as<unsigned>();
        zone.name = jzones[i][F("name")].as<String>();
        zone.enabled = true;

        if (jzones[i][F("type")].as<String>() == "cool") {
            zone.type = CLIMATE_TYPE_COOL;
        } else if (jzones[i][F("type")].as<String>() == "heat") {
            zone.type = CLIMATE_TYPE_HEAT;
        }

        Gpio.getPinById(jzones[i][F("relay")].as<unsigned>(), &zone.relay);
        Gpio.getPinById(jzones[i][F("button")].as<unsigned>(), &zone.button);

        ClimateCtrl.setZone(zone.id - 1, &zone);
    }

    /*
     * Security controller
     */

    auto jsecurity = jctrls["security"];
    SecurityCtrl.setEnabled(jsecurity["enabled"].as<bool>());
    Gpio.getPinById(jsecurity[F("relay")].as<unsigned>(), SecurityCtrl.getRelay());
    jsensors = jsecurity["sensors"];
    
    for (size_t i = 0; i < jsensors.size(); i++) {
        SecuritySensor  sensor;

        memset(&sensor, 0x0, sizeof(SecuritySensor));
        sensor.id = jsensors[i][F("id")].as<unsigned>();
        sensor.name = jsensors[i][F("name")].as<String>();

        if (jsensors[i][F("type")].as<String>() == "reed") {
            sensor.type = SECURITY_SENSOR_REED;
        } else if (jsensors[i][F("type")].as<String>() == "pir") {
            sensor.type = SECURITY_SENSOR_PIR;
        }
        Gpio.getPinById(jsensors[i][F("pin")].as<unsigned>(), &sensor.pin);

        SecurityCtrl.setSensor(sensor.id - 1, &sensor);
    }

    JsonArray jkeys = jsecurity["keys"];
    
    for (size_t i = 0; i < jkeys.size(); i++) {
        SecurityKey  key;

        key.id = jkeys[i][F("id")].as<unsigned>();
        key.name = jkeys[i][F("name")].as<String>();
        key.enabled = true;
        key.serial = strtoull(jkeys[i][F("serial")].as<String>().c_str(), NULL, 16);

        SecurityCtrl.setKey(key.id - 1, &key);
    }

    /*
     * Tank configurations
     */
    
    auto jtank = jctrls["tank"];
    TankCtrl.setEnabled(jtank["enabled"].as<bool>());

    JsonArray jtanks = jtank["tanks"];
    for (size_t i = 0; i < jtanks.size(); i++) {
        Tank tank;

        memset(&tank, 0x0, sizeof(Tank));
        tank.id = jtanks[i][F("id")].as<unsigned>();
        tank.name = jtanks[i][F("name")].as<String>();
        tank.enabled = true;
        Gpio.getPinById(jtanks[i][F("pump")].as<unsigned>(), &tank.pump);
        Gpio.getPinById(jtanks[i][F("valve")].as<unsigned>(), &tank.valve);
        Gpio.getPinById(jtanks[i][F("levels")][0].as<unsigned>(), &tank.levels[0]);
        Gpio.getPinById(jtanks[i][F("levels")][1].as<unsigned>(), &tank.levels[1]);
        Gpio.getPinById(jtanks[i][F("levels")][2].as<unsigned>(), &tank.levels[2]);

        TankCtrl.setTank(tank.id - 1, &tank);
    }

    doc.clear();
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

    auto jclock = doc[F("clock")];
    jclock[F("utc")] = Clock.getUTC();

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
     * Controllers configs
     */

    auto jctrls = doc[F("controllers")];

    /*
     * Socket controller
     */

    auto jsock = jctrls[F("socket")];
    jsock[F("enabled")] = SocketCtrl.getEnabled();

    auto jsocks = jsock[F("sockets")];
    
    std::vector<Socket *> socks;
    SocketCtrl.getEnabledSockets(socks);

    for (size_t i = 0; i < socks.size(); i++) {
        jsocks[i][F("id")] = socks[i]->id;
        jsocks[i][F("name")] = socks[i]->name;
        (socks[i]->button == nullptr) ? jsocks[i][F("button")] = 0 : jsocks[i][F("button")] = socks[i]->button->id;
        (socks[i]->relay == nullptr) ? jsocks[i][F("relay")] = 0 : jsocks[i][F("relay")] = socks[i]->relay->id;
        (socks[i]->led == nullptr) ? jsocks[i][F("led")] = 0 : jsocks[i][F("led")] = socks[i]->led->id;
    }

    /*
     * Meteo controller
     */

    auto jmeteo = jctrls["meteo"];
    jmeteo["enabled"] = MeteoCtrl.getEnabled();
    auto jmsens = jmeteo["sensors"];
    
    std::vector<MeteoSensor *> sens;
    MeteoCtrl.getSensors(true, sens);

    for (size_t i = 0; i < sens.size(); i++) {
        jmsens[i][F("id")] = sens[i]->id;
        jmsens[i][F("name")] = sens[i]->name;

        switch (sens[i]->type) {
            case METEO_SENSOR_DS18B20:
                jmsens[i][F("type")] = String("ds18b20");
                jmsens[i][F("addr")] = String(sens[i]->addr, 16);
                break;

            case METEO_SENSOR_DHT22:
                jmsens[i][F("type")] = String("dht22");
                break;

            case METEO_SENSOR_AM2302:
                jmsens[i][F("type")] = String("am2302");
                break;

            case METEO_SENSOR_BME280:
                jmsens[i][F("type")] = String("bme280");
                break;
        }

        (sens[i]->pin == nullptr) ? jmsens[i][F("pin")] = 0 : jmsens[i][F("pin")] = sens[i]->pin->id;
    }

    /*
     * Climate controller
     */

    auto jclimate = jctrls["climate"];
    jclimate["enabled"] = ClimateCtrl.getEnabled();
    auto jzones = jclimate["zones"];
    
    std::vector<ClimateZone *> zones;
    ClimateCtrl.getZones(true, zones);

    for (size_t i = 0; i < zones.size(); i++) {
        jzones[i][F("id")] = zones[i]->id;
        jzones[i][F("name")] = zones[i]->name;
        
        switch (zones[i]->type) {
            case CLIMATE_TYPE_COOL:
                jzones[i][F("type")] = "cool";
                break;

            case CLIMATE_TYPE_HEAT:
                jzones[i][F("type")] = "heat";
                break;
        }

        (zones[i]->relay == nullptr) ? jzones[i][F("relay")] = 0 : jzones[i][F("relay")] = zones[i]->relay->id;
        (zones[i]->button == nullptr) ? jzones[i][F("button")] = 0 : jzones[i][F("button")] = zones[i]->button->id;
    }

    /*
     * Security controller
     */

    auto jsecurity = jctrls["security"];
    jsecurity["enabled"] = SecurityCtrl.getEnabled();
    (SecurityCtrl.getRelay() == nullptr) ? jsecurity[F("relay")] = 0 : jsecurity[F("relay")] = (*SecurityCtrl.getRelay())->id;
    auto jsensors = jsecurity["sensors"];
    
    std::vector<SecuritySensor *> sensors;
    SecurityCtrl.getEnabledSensors(sensors);

    for (size_t i = 0; i < zones.size(); i++) {
        jsensors[i][F("id")] = zones[i]->id;
        jsensors[i][F("name")] = zones[i]->name;
        
        switch (sensors[i]->type) {
            case SECURITY_SENSOR_REED:
                jsensors[i][F("type")] = "reed";
                break;

            case SECURITY_SENSOR_PIR:
                jsensors[i][F("type")] = "pir";
                break;
        }

        (sensors[i]->pin == nullptr) ? jsensors[i][F("pin")] = 0 : jsensors[i][F("pin")] = sensors[i]->pin->id;
    }

    auto jkeys = jsecurity["keys"];
    std::vector<SecurityKey *> keys;
    SecurityCtrl.getEnabledKeys(keys);

    for (size_t i = 0; i < keys.size(); i++) {
        jkeys[i][F("id")] = keys[i]->id;
        jkeys[i][F("name")] = keys[i]->name;
        jkeys[i][F("serial")] = String(keys[i]->serial, 16);
    }

    /*
     * Tank controller
     */

    auto jtank = jctrls["tank"];
    jtank["enabled"] = TankCtrl.getEnabled();
    auto jtanks = jtank["tanks"];
    
    std::vector<Tank *> tanks;
    TankCtrl.getEnabledTanks(tanks);

    for (size_t i = 0; i < tanks.size(); i++) {
        jtanks[i][F("id")] = tanks[i]->id;
        jtanks[i][F("name")] = tanks[i]->name;

        (tanks[i]->pump == nullptr) ? jtanks[i][F("pump")] = 0 : jtanks[i][F("pump")] = tanks[i]->pump->id;
        (tanks[i]->valve == nullptr) ? jtanks[i][F("valve")] = 0 : jtanks[i][F("valve")] = tanks[i]->valve->id;
        (tanks[i]->valve == nullptr) ? jtanks[i][F("valve")] = 0 : jtanks[i][F("valve")] = tanks[i]->valve->id;
        (tanks[i]->levels[0] == nullptr) ? jtanks[i][F("levels")][0] = 0 : jtanks[i][F("levels")][0] = tanks[i]->levels[0]->id;
        (tanks[i]->levels[1] == nullptr) ? jtanks[i][F("levels")][1] = 0 : jtanks[i][F("levels")][1] = tanks[i]->levels[1]->id;
        (tanks[i]->levels[2] == nullptr) ? jtanks[i][F("levels")][2] = 0 : jtanks[i][F("levels")][2] = tanks[i]->levels[2]->id;
    }

    return true;
}

ConfigsSource ConfigsClass::getSource() const
{
    return _src;
}

ConfigsClass Configs;
