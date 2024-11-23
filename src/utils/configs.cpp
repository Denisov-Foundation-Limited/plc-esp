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
#include "net/tgbot.hpp"
#include "net/apiserver.hpp"
#include "controllers/meteo/meteo.hpp"
#include "controllers/meteo/sensors/msensor.hpp"
#include "controllers/meteo/sensors/ds18b20.hpp"
#include "controllers/ctrls.hpp"
#include "net/core/eth.hpp"
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
    Interface   *iface = nullptr;

    _initInterfaces();
    Log.info(LOG_MOD_CFG, "Interfaces initialized");

    /*if ((iface = Interfaces.getInterface(F("spi-sd"))) == nullptr) {
        Log.warning(LOG_MOD_CFG, F("Interface SDcard SPI not found"));
    }
    auto *spiSD = static_cast<IfSPI *>(iface);

    SPI.end();
    SPI.begin(spiSD->getPin(SPI_PIN_SCK), spiSD->getPin(SPI_PIN_MISO), spiSD->getPin(SPI_PIN_MOSI));

    if (SD.begin(spiSD->getPin(SPI_PIN_SS), SPI, spiSD->getFrequency())) {
        Log.info(LOG_MOD_CFG, F("SD card found. Reading files"));
        if (!SD.exists(CONFIGS_STARTUP_FILE))
        {
            _src = CFG_SRC_SD;
            return _initDevice();
        } else {
            return _readAll(CFG_SRC_SD);
        }
    }*/

    Log.warning(LOG_MOD_CFG, F("SD card not found. Trying to read from flash memory"));

    _src = CFG_SRC_FLASH;

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
    Interface *iface = nullptr;

    Log.info(LOG_MOD_CFG, String(F("Selected board: ")) + String(ActiveBoard.name));

    /* Extenders */

    for (auto ext : ActiveBoard.extenders) {
        if (ext.addr == 0)
            break;
        Extenders.addExtender(new Extender(static_cast<ExtenderId>(ext.id), ext.addr));
    }

    /* GPIO Interfaces */

    for (auto gpio : ActiveBoard.interfaces.gpio) {
        if (gpio.pin == 0)
            break;
        Interfaces.addInterface(new IfGPIO(gpio.name, static_cast<GpioType>(gpio.type), gpio.pin, static_cast<GpioMode>(gpio.mode),
                                static_cast<GpioPull>(gpio.pull), static_cast<ExtenderId>(gpio.extId)));
    }

    /* OneWire Interface */

    for (auto ow : ActiveBoard.interfaces.onewire) {
        if (ow.pin == 0)
            break;
        Interfaces.addInterface(new IfOneWire(ow.name, ow.pin));
    }

    /* I2C Interface */

    for (auto i2c : ActiveBoard.interfaces.i2c) {
        if (i2c.sda == 0)
            break;
        Interfaces.addInterface(new IfI2C(i2c.name, i2c.sda, i2c.scl));
    }

    /* SPI Interface */

    for (auto spi : ActiveBoard.interfaces.spi) {
        if (spi.miso == 0)
            break;
        Interfaces.addInterface(new IfSPI(spi.name, spi.miso, spi.mosi, spi.sck, spi.ss, spi.freq));
    }

    /* UART Interface */

    for (auto uart : ActiveBoard.interfaces.uart) {
        if (uart.rx == 0)
            break;
        Interfaces.addInterface(new IfUART(uart.name, uart.rx, uart.tx, uart.rate));
    }

    /* Wi-Fi setup */

    if ((iface = Interfaces.getInterface(ActiveBoard.net.led)) == nullptr) {
        if (ActiveBoard.net.led != "") { Log.error(LOG_MOD_CFG, F("Interface NET LED not found")); }
    }
    Wireless.setStatusLed(static_cast<IfGPIO *>(iface));

    /* PLC setup */

    if ((iface = Interfaces.getInterface(ActiveBoard.plc.alarm)) == nullptr) {
        if (ActiveBoard.plc.alarm != "") { Log.error(LOG_MOD_CFG, F("Interface PLC Alarm not found")); }
    }
    Plc.setPin(PLC_GPIO_ALARM_LED, static_cast<IfGPIO *>(iface));
    if ((iface = Interfaces.getInterface(ActiveBoard.plc.status)) == nullptr) {
        if (ActiveBoard.plc.status != "") { Log.error(LOG_MOD_CFG, F("Interface PLC Status not found")); }
    }
    Plc.setPin(PLC_GPIO_STATUS_LED, static_cast<IfGPIO *>(iface));
    if ((iface = Interfaces.getInterface(ActiveBoard.plc.buzzer)) == nullptr) {
        if (ActiveBoard.plc.buzzer != "") { Log.warning(LOG_MOD_CFG, F("Interface PLC Buzzer not found")); }
    }
    Plc.setPin(PLC_GPIO_BUZZER, static_cast<IfGPIO *>(iface));

    /* GSM Modem setup */

    if ((iface = Interfaces.getInterface(ActiveBoard.net.gsm.uart)) == nullptr) {
        if (ActiveBoard.net.gsm.uart != "") { Log.error(LOG_MOD_CFG, F("Interface GSM UART not found")); }
    }
    GsmModem.setUart(static_cast<IfUART *>(iface));

    /* Ethernet MAC addr */

    EthernetCard.setMAC((byte *)ActiveBoard.net.ethernet.mac_addr);
    if ((iface = Interfaces.getInterface(ActiveBoard.net.ethernet.spi)) == nullptr) {
        if (ActiveBoard.net.ethernet.spi != "") { Log.error(LOG_MOD_CFG, F("Interface Ethernet SPI not found")); }
    }
    EthernetCard.setInterface(ETH_IF_SPI, iface);
    if ((iface = Interfaces.getInterface(ActiveBoard.net.ethernet.irq)) == nullptr) {
        if (ActiveBoard.net.ethernet.irq != "") { Log.warning(LOG_MOD_CFG, F("Interface Ethernet IRQ not found")); }
    }
    EthernetCard.setInterface(ETH_IF_IRQ, iface);
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
    Log.info(LOG_MOD_CFG, F("Configs not found. Init new device"));

    /* Wi-Fi setup */

    Wireless.setEnabled(ActiveBoard.net.wifi.enabled);
    Wireless.setHostname(ActiveBoard.net.hostname);
    Wireless.setCreds(ActiveBoard.net.wifi.ssid, ActiveBoard.net.wifi.passwd);

    /* Ethernet setup */
    
    EthernetCard.setEnabled(ActiveBoard.net.ethernet.enabled);
    EthernetCard.setHostname(F(ActiveBoard.net.hostname));

    /* TgBot setup */

    TgBot.setPollMode(fb::Poll::Long, 20000);

    return true;
}

bool ConfigsClass::_readAll(ConfigsSource src)
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

    JsonArray exts = doc[F("extenders")];
    for (uint8_t i = 0; i < exts.size(); i++) {
        Extenders.addExtender(new Extender(static_cast<ExtenderId>(exts[i]["id"]), exts[i]["addr"], true));
    }

    /*
     * GPIO configurations
     */

    JsonArray jifaces = doc[F("interfaces")];
    for (uint8_t i = 0; i < jifaces.size(); i++) {
        if (jifaces[i][F("type")] == "gpio" || jifaces[i][F("type")] == "relay" || jifaces[i][F("type")] == "input") {
            GpioMode    mode = GPIO_MOD_INPUT;
            GpioPull    pull = GPIO_PULL_NONE;
            GpioType    type = GPIO_TYPE_GEN;

            if (jifaces[i][F("mode")] == "input") {
                mode = GPIO_MOD_INPUT;
            } else if (jifaces[i][F("mode")] == "output") {
                mode = GPIO_MOD_OUTPUT;
            } else {
                Log.error(LOG_MOD_CFG, String(F("GPIO mode not found: ")) + jifaces[i][F("mode")].as<String>());
                doc.clear();
                return false;
            }

            if (jifaces[i][F("pull")] == "none") {
                pull = GPIO_PULL_NONE;
            } else if (jifaces[i][F("pull")] == "up") {
                pull = GPIO_PULL_UP;
            } else if (jifaces[i][F("pull")] == "down") {
                pull = GPIO_PULL_DOWN;
            } else {
                Log.error(LOG_MOD_CFG, String(F("GPIO pull not found: ")) + jifaces[i][F("pull")].as<String>());
                doc.clear();
                return false;
            }

            if (jifaces[i][F("type")] == "general") {
                type = GPIO_TYPE_GEN;
            } else if (jifaces[i][F("type")] == "relay") {
                type = GPIO_TYPE_RELAY;
            } else if (jifaces[i][F("type")] == "dinput") {
                type = GPIO_TYPE_DINPUT;
            }
            Interfaces.addInterface(new IfGPIO(jifaces[i][F("name")], type, jifaces[i][F("pin")], mode, pull, jifaces[i][F("ext")], true));
        } else if (jifaces[i][F("type")] == "ow") {
            Interfaces.addInterface(new IfOneWire(jifaces[i][F("name")], jifaces[i][F("pin")], true));
        } else if (jifaces[i][F("type")] == "spi") {
            Interfaces.addInterface(new IfSPI(jifaces[i][F("name")], jifaces[i][F("miso")], jifaces[i][F("mosi")],
                                                jifaces[i][F("sck")], jifaces[i][F("ss")], jifaces[i][F("freq")], true));
        } else if (jifaces[i][F("type")] == "i2c") {
            Interfaces.addInterface(new IfI2C(jifaces[i][F("name")], jifaces[i][F("sda")], jifaces[i][F("scl")], true));
        } else if (jifaces[i][F("type")] == "uart") {
            Interfaces.addInterface(new IfUART(jifaces[i][F("name")], jifaces[i][F("rx")], jifaces[i][F("tx")], jifaces[i][F("rate")], true));
        } else {
            Log.error(LOG_MOD_CFG, String(F("Interface type unknown: ")) + jifaces[i][F("type")].as<String>());
            doc.clear();
            return false;
        }
    }

    /*
     * Network configurations
     */

    auto jnet = doc[F("network")];
    auto jeth = jnet[F("ethernet")];

    EthernetCard.setEnabled(jeth[F("enabled")]);
    EthernetCard.setHostname(jeth[F("hostname")]);
    EthernetCard.setDHCP(jeth[F("dhcp")]);
    if (!EthernetCard.getDHCP()) {
        EthernetCard.setAddress(ETH_ADDR_IP, jeth[F("static")][F("ip")]);
        EthernetCard.setAddress(ETH_ADDR_SUBNET, jeth[F("static")][F("subnet")]);
        EthernetCard.setAddress(ETH_ADDR_GATEWAY, jeth[F("static")][F("gateway")]);
        EthernetCard.setAddress(ETH_ADDR_DNS, jeth[F("static")][F("dns")]);
    }

    auto jwifi = jnet[F("wifi")];
    Wireless.setCreds(jwifi[F("ssid")], jwifi[F("passwd")]);
    Wireless.setHostname(jwifi[F("hostname")]);
    Wireless.setAP(jwifi[F("ap")]);
    pin = Interfaces.getInterface(jwifi[F("iface")]);
    if (pin != nullptr && pin->getType() == IF_TYPE_GPIO) {
        Wireless.setStatusLed(static_cast<IfGPIO *>(pin));
    } else {
        Log.warning(LOG_MOD_CFG, F("WiFi status led GPIO interface not found"));
    }
    Wireless.setStatusLed(static_cast<IfGPIO *>(pin));
    Wireless.setEnabled(jwifi[F("enabled")]);

    /*
     * GSM modem configurations
     */

    pin = Interfaces.getInterface(doc[F("gsm")][F("iface")]);
    if (pin != nullptr && pin->getType() == IF_TYPE_UART) {
        GsmModem.setUart(static_cast<IfUART *>(pin));
    } else {
        Log.warning(LOG_MOD_CFG, F("GsmModem UART interface not found"));
    }
    GsmModem.setEnabled(doc[F("gsm")][F("enabled")]);
    GsmModem.begin();

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
        auto user = new TgUser();
        user->name = usr[F("name")].as<String>();
        user->chatId = usr[F("id")];
        user->notify = usr[F("notify")];
        user->admin = usr[F("admin")];
        TgBot.addUser(user);
    }
    TgBot.setEnabled(jtgbot[F("enabled")]);

    /*
     * Controllers configurations
     */

    JsonArray jctrls = doc[F("controllers")];
    for (auto jctrl : jctrls) {
        if (jctrl[F("type")] == "meteo") {
            auto *meteo = new MeteoCtrl(jctrl[F("name")].as<String>());
            auto *iface = Interfaces.getInterface(jctrl[F("ow")]);
            if (iface != nullptr && iface->getType() == IF_TYPE_OW) {
                meteo->setOneWire(static_cast<IfOneWire *>(iface));
            } else {
                Log.warning(LOG_MOD_CFG, String(F("Meteo controller ")) +
                        meteo->getName() + String(F(" OneWire interface not found")));
            }
            JsonArray jsens = jctrl[F("sensors")];
            for (auto js : jsens) {
                if (js[F("type")] == "ds18b20") {
                    auto ds = new Ds18b20(js[F("name")].as<String>());
                    ds->setId(js[F("id")]);
                    ds->setEnabled(js[F("enabled")]);
                    meteo->addSensor(ds);
                }
            }
            meteo->setEnabled(jctrl[F("enabled")]);
            Controllers.addController(meteo);
        } else if (jctrl[F("type")] == "socket") {
            auto        *socket = new SocketCtrl(jctrl[F("name")].as<String>());

            JsonArray jsock = jctrl[F("sockets")];
            for (auto js : jsock) {
                auto s = new Socket(js[F("name")].as<String>());

                auto *iface = Interfaces.getInterface(js[F("relay")]);
                if (iface != nullptr) {
                    s->setInterface(SOCK_IF_RELAY, iface);
                } else {
                    if (js[F("relay")] != "") {
                        Log.error(LOG_MOD_CFG, String(F("Socket interface not found Relay: ")) + js[F("relay")].as<String>());
                    }
                }
                iface = Interfaces.getInterface(js[F("button")]);
                if (iface != nullptr) {
                    s->setInterface(SOCK_IF_BUTTON, iface);
                } else {
                    if (js[F("button")] != "") {
                        Log.error(LOG_MOD_CFG, String(F("Socket interface not found Button: ")) + js[F("button")].as<String>());
                    }
                }
                s->setController(socket);
                socket->addSocket(s);
            }
            socket->setEnabled(jctrl[F("enabled")]);
            Controllers.addController(socket);
        } else if (jctrl[F("type")] == "security") {
            
        } else if (jctrl[F("type")] == "tank") {
            
        }
    }

    doc.clear();
    return true;
}

bool ConfigsClass::loadStates()
{
    for (auto *ctrl : Controllers.getControllers()) {
        switch (ctrl->getType()) {
            case CTRL_TYPE_SOCKET:
                {
                    SocketDB    db;
                    SocketCtrl *sockCtrl = static_cast<SocketCtrl *>(ctrl);

                    db.loadFromFile(String(su::SH(sockCtrl->getName().c_str()), HEX) + ".json");
                    if (db.isLoad()) {
                        for (auto *socket : sockCtrl->getSockets()) {
                            bool status = false;
                            if (db.getStatus(socket->getName(), status)) {
                                socket->setStatus(status, false);
                            }
                        }
                        db.clear();
                        db.close();
                    }
                }
                break;
        }
    }
    return true;
}

bool ConfigsClass::_generateRunning(JsonDocument &doc)
{
    /*
     * PLC general configurations
     */

    auto jplc = doc[F("plc")];
    jplc[F("name")] = Plc.getName();

    /*
     * Network configurations
     */

    auto jnet = doc[F("network")];
    auto jeth = jnet[F("ethernet")];
    jeth[F("enabled")] = EthernetCard.getEnabled();
    jeth[F("hostname")] = EthernetCard.getHostname();
    jeth[F("dhcp")] = EthernetCard.getDHCP();
    if (!EthernetCard.getDHCP()) {
        jeth[F("static")][F("ip")] = EthernetCard.getAddress(ETH_ADDR_IP).toString();
        jeth[F("static")][F("subnet")] = EthernetCard.getAddress(ETH_ADDR_SUBNET).toString();
        jeth[F("static")][F("gateway")] = EthernetCard.getAddress(ETH_ADDR_GATEWAY).toString();
        jeth[F("static")][F("dns")] = EthernetCard.getAddress(ETH_ADDR_DNS).toString();
    }

    auto jwifi = jnet[F("wifi")];
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
     * Extenders configurations
     */

    auto jext = doc[F("extenders")];
    for (uint8_t i = 0; i < Extenders.getExtenders().size(); i++) {
        auto e = Extenders.getExtenders()[i];
        if (!e->getExtended())
         continue;
        jext[i][F("id")] = e->getID();
        jext[i][F("addr")] = e->getAddr();
    }

    /*
     * GPIO configurations
     */

    auto jifaces = doc[F("interfaces")];
    for (uint8_t i = 0; i < Interfaces.getInterfaces().size(); i++) {
        auto p = Interfaces.getInterfaces()[i];

        if (!p->getExtended())
            continue;

        jifaces[i][F("name")] = p->getName();

        switch (p->getType()) {
            case IF_TYPE_GPIO:
                jifaces[i][F("type")] = F("gpio");
                break;

            case IF_TYPE_OW:
                jifaces[i][F("type")] = F("ow");
                break;

            case IF_TYPE_I2C:
                jifaces[i][F("type")] = F("i2c");
                break;

            case IF_TYPE_SPI:
                jifaces[i][F("type")] = F("spi");
                break;

            case IF_TYPE_UART:
                jifaces[i][F("type")] = F("uart");
                break;
        }

        if (p->getType() == IF_TYPE_GPIO) {
            auto gpio = static_cast<IfGPIO *>(p);
            switch (gpio->getPinType()) {
                case GPIO_TYPE_GEN:
                    jifaces[i][F("type")] = F("general");
                    break;

                case GPIO_TYPE_RELAY:
                    jifaces[i][F("type")] = F("relay");
                    break;

                case GPIO_TYPE_DINPUT:
                    jifaces[i][F("type")] = F("dinput");
                    break;
            }
            jifaces[i][F("pin")] = gpio->getPin();
            switch (gpio->getMode()) {
                case GPIO_MOD_INPUT:
                    jifaces[i][F("mode")] = F("input");
                    break;

                case GPIO_MOD_OUTPUT:
                    jifaces[i][F("mode")] = F("output");
                    break;
            }
            switch (gpio->getPull()) {
                case GPIO_PULL_NONE:
                    jifaces[i][F("pull")] = F("none");
                    break;

                case GPIO_PULL_UP:
                    jifaces[i][F("pull")] = F("up");
                    break;

                case GPIO_PULL_DOWN:
                    jifaces[i][F("pull")] = F("down");
                    break;
            }
            jifaces[i][F("ext")] = gpio->getExtId();
        } else if (p->getType() == IF_TYPE_SPI) {
            auto spi = static_cast<IfSPI *>(p);
            jifaces[i][F("miso")] = spi->getPin(SPI_PIN_MISO);
            jifaces[i][F("mosi")] = spi->getPin(SPI_PIN_MOSI);
            jifaces[i][F("sck")] = spi->getPin(SPI_PIN_SCK);
            jifaces[i][F("ss")] = spi->getPin(SPI_PIN_SS);
            jifaces[i][F("freq")] = spi->getFrequency();
        } else if (p->getType() == IF_TYPE_OW) {
            auto ow = static_cast<IfOneWire *>(p);
            jifaces[i][F("pin")] = ow->getPin();
        } else if (p->getType() == IF_TYPE_I2C) {
            auto i2c = static_cast<IfI2C *>(p);
            jifaces[i][F("sda")] = i2c->getPin(I2C_PIN_SDA);
            jifaces[i][F("scl")] = i2c->getPin(I2C_PIN_SCL);
        } else if (p->getType() == IF_TYPE_UART) {
            auto uart = static_cast<IfUART *>(p);
            jifaces[i][F("rx")] = uart->getPin(UART_PIN_RX);
            jifaces[i][F("tx")] = uart->getPin(UART_PIN_TX);
            jifaces[i][F("rate")] = uart->getRate();
        }
    }

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
    for (auto *usr : TgBot.getUsers()) {
        jusers[k][F("name")] = usr->name;
        jusers[k][F("id")] = usr->chatId;
        jusers[k][F("notify")] = usr->notify;
        jusers[k][F("admin")] = usr->admin;
        k++;
    }

    /*
     * Controllers
     */

    auto jctrls = doc[F("controllers")];
    unsigned i = 0;

    for (auto *c : Controllers.getControllers()) {
        if (c->getType() != CTRL_TYPE_METEO) {
            continue;
        }
        auto *meteo = static_cast<MeteoCtrl *>(c);
        jctrls[i][F("name")] = meteo->getName();
        jctrls[i][F("type")] = F("meteo");
        jctrls[i][F("enabled")] = meteo->getEnabled();
        jctrls[i][F("ow")] = (meteo->getOneWire() != nullptr) ? meteo->getOneWire()->getName() : "";
        unsigned j = 0;
        auto jsensors = jctrls[i][F("sensors")];
        for (auto *s : meteo->getSensors()) {
            jsensors[j][F("name")] = s->getName();
            switch (s->getType()) {
                case METEO_SENSOR_DS18: {
                        auto *ds = static_cast<Ds18b20 *>(s);
                        jsensors[j][F("type")] = F("ds18b20");
                        jsensors[j][F("id")] = ds->getId();
                        jsensors[j][F("enabled")] = ds->getEnabled();
                    }
                    break;
            }
            j++;
        }
        i++;
    }

    for (auto *c : Controllers.getControllers()) {
        if (c->getType() != CTRL_TYPE_SOCKET) {
            continue;
        }
        auto *socket = static_cast<SocketCtrl *>(c);
        jctrls[i][F("name")] = socket->getName();
        jctrls[i][F("type")] = F("socket");
        jctrls[i][F("enabled")] = socket->getEnabled();
        unsigned j = 0;
        auto jsocks = jctrls[i][F("sockets")];
        for (auto *s : socket->getSockets()) {
            jsocks[j][F("name")] = s->getName();
            jsocks[j][F("relay")] = (s->getInterface(SOCK_IF_RELAY) == nullptr) ? "" : s->getInterface(SOCK_IF_RELAY)->getName();
            jsocks[j][F("button")] = (s->getInterface(SOCK_IF_BUTTON) == nullptr) ? "" : s->getInterface(SOCK_IF_BUTTON)->getName();
            j++;
        }
        i++;
    }

    for (auto *c : Controllers.getControllers()) {
        if (c->getType() != CTRL_TYPE_SECURITY) {
            continue;
        }
        i++;
    }

    for (auto *c : Controllers.getControllers()) {
        if (c->getType() != CTRL_TYPE_TANK) {
            continue;
        }
        i++;
    }

    return true;
}

ConfigsSource ConfigsClass::getSource() const
{
    return _src;
}

ConfigsClass Configs;
