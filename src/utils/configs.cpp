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

    if ((iface = Interfaces.getInterface(F("spi-sd"))) == nullptr) {
        Log.warning(LOG_MOD_CFG, F("Interface SDcard SPI not found"));
    }
    auto *spiSD = static_cast<SPIface *>(iface);

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
    Log.info(LOG_MOD_CFG, String(F("Selected board: ")) + String(ActiveBoard.name));

    /* Extenders */

    for (auto ext : ActiveBoard.extenders) {
        if (ext.addr == 0)
            break;
        Extenders.addExtender(new Extender(static_cast<ExtenderId>(ext.id), ext.addr));
    }

    /* GPIO Interfaces */

    for (auto gpio : ActiveBoard.gpio) {
        if (gpio.pin == 0)
            break;
        Interfaces.addInterface(new GPIOIface(gpio.name, gpio.pin, static_cast<GpioMode>(gpio.mode),
                                static_cast<GpioPull>(gpio.pull), static_cast<ExtenderId>(gpio.extId)));
    }

    /* OneWire Interface */

    for (auto ow : ActiveBoard.onewire) {
        if (ow.pin == 0)
            break;
        Interfaces.addInterface(new OneWireIface(ow.name, ow.pin));
    }

    /* I2C Interface */

    for (auto i2c : ActiveBoard.i2c) {
        if (i2c.sda == 0)
            break;
        Interfaces.addInterface(new I2CIface(i2c.name, i2c.sda, i2c.scl));
    }

    /* SPI Interface */

    for (auto spi : ActiveBoard.spi) {
        if (spi.miso == 0)
            break;
        Interfaces.addInterface(new SPIface(spi.name, spi.miso, spi.mosi, spi.sck, spi.ss, spi.freq));
    }

    /* UART Interface */

    for (auto uart : ActiveBoard.uart) {
        if (uart.rx == 0)
            break;
        Interfaces.addInterface(new UARTIface(uart.name, uart.rx, uart.tx, uart.rate));
    }
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
    Interface *iface = nullptr;

    Log.info(LOG_MOD_CFG, F("Configs not found. Init new device"));

    /* GSM Modem setup */

    if ((iface = Interfaces.getInterface("uart-gsm")) == nullptr) {
        Log.warning(LOG_MOD_CFG, F("Interface uart-gsm not found"));
    }
    GsmModem.setUart(static_cast<UARTIface *>(iface));

    /* PLC setup */

    if ((iface = Interfaces.getInterface("led-alrm")) == nullptr) {
        Log.warning(LOG_MOD_CFG, F("Interface led-alrm not found"));
    }
    Plc.setPin(PLC_GPIO_ALARM_LED, static_cast<GPIOIface *>(iface));
    if ((iface = Interfaces.getInterface("led-sts")) == nullptr) {
        Log.warning(LOG_MOD_CFG, F("Interface led-sts not found"));
    }
    Plc.setPin(PLC_GPIO_STATUS_LED, static_cast<GPIOIface *>(iface));
    if ((iface = Interfaces.getInterface("buzzer")) == nullptr) {
        Log.warning(LOG_MOD_CFG, F("Interface buzzer not found"));
    }
    Plc.setPin(PLC_GPIO_BUZZER, static_cast<GPIOIface *>(iface));

    /* Wi-Fi setup */

    Wireless.setHostname(F("FCPLC"));
    if ((iface = Interfaces.getInterface("led-net")) == nullptr) {
        Log.warning(LOG_MOD_CFG, F("Interface led-net not found"));
    }
    Wireless.setStatusLed(static_cast<GPIOIface *>(iface));

    /* Ethernet setup */
    
    Ethernet.setEnabled(true);
    Ethernet.setHostname(F("FCPLC"));
    Ethernet.setMAC((byte *)ActiveBoard.mac_addr);
    if ((iface = Interfaces.getInterface("spi-eth")) == nullptr) {
        Log.warning(LOG_MOD_CFG, F("Interface Ethernet SPI not found"));
    }
    Ethernet.setInterface(ETH_IF_SPI, iface);
    if ((iface = Interfaces.getInterface("eth-irq")) == nullptr) {
        Log.warning(LOG_MOD_CFG, F("Interface Ethernet IRQ not found"));
    }
    Ethernet.setInterface(ETH_IF_IRQ, iface);

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
        if (jifaces[i][F("type")] == "gpio") {
            GpioMode    type = GPIO_MOD_INPUT;
            GpioPull    pull = GPIO_PULL_NONE;

            if (jifaces[i][F("mode")] == "input") {
                type = GPIO_MOD_INPUT;
            } else if (jifaces[i][F("mode")] == "output") {
                type = GPIO_MOD_OUTPUT;
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

            Interfaces.addInterface(new GPIOIface(jifaces[i][F("name")], jifaces[i][F("pin")], type, pull, jifaces[i][F("ext")], true));
        } else if (jifaces[i][F("type")] == "ow") {
            Interfaces.addInterface(new OneWireIface(jifaces[i][F("name")], jifaces[i][F("pin")], true));
        } else if (jifaces[i][F("type")] == "spi") {
            Interfaces.addInterface(new SPIface(jifaces[i][F("name")], jifaces[i][F("miso")], jifaces[i][F("mosi")],
                                                jifaces[i][F("sck")], jifaces[i][F("ss")], jifaces[i][F("freq")], true));
        } else if (jifaces[i][F("type")] == "i2c") {
            Interfaces.addInterface(new I2CIface(jifaces[i][F("name")], jifaces[i][F("sda")], jifaces[i][F("scl")], true));
        } else if (jifaces[i][F("type")] == "uart") {
            Interfaces.addInterface(new UARTIface(jifaces[i][F("name")], jifaces[i][F("rx")], jifaces[i][F("tx")], jifaces[i][F("rate")], true));
        } else {
            Log.error(LOG_MOD_CFG, String(F("Interface type unknown: ")) + jifaces[i][F("type")].as<String>());
            doc.clear();
            return false;
        }
    }

    /*
     * PLC general configurations
     */

    pin = Interfaces.getInterface(doc[F("plc")][F("iface")][F("status")]);
    if (pin != nullptr && pin->getType() == INT_TYPE_GPIO) {
        Plc.setPin(PLC_GPIO_STATUS_LED, static_cast<GPIOIface *>(pin));
    } else {
        Log.warning(LOG_MOD_CFG, F("PLC status LED GPIO interface not found"));
    }

    pin = Interfaces.getInterface(doc[F("plc")][F("iface")][F("alarm")]);
    if (pin != nullptr && pin->getType() == INT_TYPE_GPIO) {
        Plc.setPin(PLC_GPIO_ALARM_LED, static_cast<GPIOIface *>(pin));
    } else {
        Log.warning(LOG_MOD_CFG, F("PLC alarm LED GPIO interface not found"));
    }

    pin = Interfaces.getInterface(doc[F("plc")][F("iface")][F("buzzer")]);
    if (pin != nullptr && pin->getType() == INT_TYPE_GPIO) {
        Plc.setPin(PLC_GPIO_BUZZER, static_cast<GPIOIface *>(pin));
    } else {
        Log.warning(LOG_MOD_CFG, F("PLC buzzer GPIO interface not found"));
    }

    /*
     * Network configurations
     */

    auto jnet = doc[F("network")];
    auto jeth = jnet[F("ethernet")];

    Ethernet.setEnabled(jeth[F("enabled")]);
    Ethernet.setHostname(jeth[F("hostname")]);
    Ethernet.setDHCP(jeth[F("dhcp")]);
    if (!Ethernet.getDHCP()) {
        Ethernet.setAddress(ETH_ADDR_IP, jeth[F("static")][F("ip")]);
        Ethernet.setAddress(ETH_ADDR_SUBNET, jeth[F("static")][F("subnet")]);
        Ethernet.setAddress(ETH_ADDR_GATEWAY, jeth[F("static")][F("gateway")]);
        Ethernet.setAddress(ETH_ADDR_DNS, jeth[F("static")][F("dns")]);
    }
    pin = Interfaces.getInterface(jeth[F("iface")][F("spi")]);
    if (pin != nullptr && pin->getType() == INT_TYPE_SPI) {
        Ethernet.setInterface(ETH_IF_SPI, pin);
    } else {
        Log.warning(LOG_MOD_CFG, F("Ethernet SPI interface not found"));
    }
    pin = Interfaces.getInterface(jeth[F("iface")][F("irq")]);
    if (pin != nullptr && pin->getType() == INT_TYPE_GPIO) {
        Ethernet.setInterface(ETH_IF_IRQ, pin);
    } else {
        Log.warning(LOG_MOD_CFG, F("Ethernet IRQ interface not found"));
    }

    auto jwifi = jnet[F("wifi")];
    Wireless.setCreds(jwifi[F("ssid")], jwifi[F("passwd")]);
    Wireless.setHostname(jwifi[F("hostname")]);
    Wireless.setAP(jwifi[F("ap")]);
    pin = Interfaces.getInterface(jwifi[F("iface")]);
    if (pin != nullptr && pin->getType() == INT_TYPE_GPIO) {
        Wireless.setStatusLed(static_cast<GPIOIface *>(pin));
    } else {
        Log.warning(LOG_MOD_CFG, F("WiFi status led GPIO interface not found"));
    }
    Wireless.setStatusLed(static_cast<GPIOIface *>(pin));
    Wireless.setEnabled(jwifi[F("enabled")]);

    /*
     * GSM modem configurations
     */

    pin = Interfaces.getInterface(doc[F("gsm")][F("iface")]);
    if (pin != nullptr && pin->getType() == INT_TYPE_UART) {
        GsmModem.setUart(static_cast<UARTIface *>(pin));
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
            auto *meteo = new Meteo(jctrl[F("name")].as<String>());
            auto *iface = Interfaces.getInterface(jctrl[F("ow")]);
            if (iface != nullptr && iface->getType() == INT_TYPE_OW) {
                meteo->setOneWire(static_cast<OneWireIface *>(iface));
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
            
        } else if (jctrl[F("type")] == "security") {
            
        } else if (jctrl[F("type")] == "tank") {
            
        }
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
    jplc[F("iface")][F("alarm")] = (Plc.getPin(PLC_GPIO_ALARM_LED) == nullptr) ? "" : Plc.getPin(PLC_GPIO_ALARM_LED)->getName();
    jplc[F("iface")][F("status")] = (Plc.getPin(PLC_GPIO_STATUS_LED) == nullptr) ? "" : Plc.getPin(PLC_GPIO_STATUS_LED)->getName();
    jplc[F("iface")][F("buzzer")] = (Plc.getPin(PLC_GPIO_BUZZER) == nullptr) ? "" : Plc.getPin(PLC_GPIO_BUZZER)->getName();

    /*
     * Network configurations
     */

    auto jnet = doc[F("network")];
    auto jeth = jnet[F("ethernet")];
    jeth[F("enabled")] = Ethernet.getEnabled();
    jeth[F("hostname")] = Ethernet.getHostname();
    jeth[F("dhcp")] = Ethernet.getDHCP();
    if (!Ethernet.getDHCP()) {
        jeth[F("static")][F("ip")] = Ethernet.getAddress(ETH_ADDR_IP).toString();
        jeth[F("static")][F("subnet")] = Ethernet.getAddress(ETH_ADDR_SUBNET).toString();
        jeth[F("static")][F("gateway")] = Ethernet.getAddress(ETH_ADDR_GATEWAY).toString();
        jeth[F("static")][F("dns")] = Ethernet.getAddress(ETH_ADDR_DNS).toString();
    }
    jeth[F("iface")][F("spi")] = (Ethernet.getInterface(ETH_IF_SPI) == nullptr) ? "" : Ethernet.getInterface(ETH_IF_SPI)->getName();
    jeth[F("iface")][F("irq")] = (Ethernet.getInterface(ETH_IF_IRQ) == nullptr) ? "" : Ethernet.getInterface(ETH_IF_IRQ)->getName();

    auto jwifi = jnet[F("wifi")];
    jwifi[F("enabled")] = Wireless.getEnabled();
    jwifi[F("hostname")] = Wireless.getHostname();
    jwifi[F("ssid")] = Wireless.getSSID();
    jwifi[F("passwd")] = Wireless.getPasswd();
    jwifi[F("ap")] = Wireless.getAP();
    jwifi[F("iface")] = (Wireless.getStatusLed() == nullptr) ? "" : Wireless.getStatusLed()->getName();

    /*
     * GSM modem configurations
     */

    auto jgsm = doc[F("gsm")];
    jgsm[F("enabled")] = GsmModem.getEnabled();
    jgsm[F("iface")] = (GsmModem.getUart() == nullptr) ? "" : GsmModem.getUart()->getName();

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
            case INT_TYPE_GPIO:
                jifaces[i][F("type")] = F("gpio");
                break;

            case INT_TYPE_OW:
                jifaces[i][F("type")] = F("ow");
                break;

            case INT_TYPE_I2C:
                jifaces[i][F("type")] = F("i2c");
                break;

            case INT_TYPE_SPI:
                jifaces[i][F("type")] = F("spi");
                break;

            case INT_TYPE_UART:
                jifaces[i][F("type")] = F("uart");
                break;
        }

        if (p->getType() == INT_TYPE_GPIO) {
            auto gpio = static_cast<GPIOIface *>(p);
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
        } else if (p->getType() == INT_TYPE_SPI) {
            auto spi = static_cast<SPIface *>(p);
            jifaces[i][F("miso")] = spi->getPin(SPI_PIN_MISO);
            jifaces[i][F("mosi")] = spi->getPin(SPI_PIN_MOSI);
            jifaces[i][F("sck")] = spi->getPin(SPI_PIN_SCK);
            jifaces[i][F("ss")] = spi->getPin(SPI_PIN_SS);
            jifaces[i][F("freq")] = spi->getFrequency();
        } else if (p->getType() == INT_TYPE_OW) {
            auto ow = static_cast<OneWireIface *>(p);
            jifaces[i][F("pin")] = ow->getPin();
        } else if (p->getType() == INT_TYPE_I2C) {
            auto i2c = static_cast<I2CIface *>(p);
            jifaces[i][F("sda")] = i2c->getPin(I2C_PIN_SDA);
            jifaces[i][F("scl")] = i2c->getPin(I2C_PIN_SCL);
        } else if (p->getType() == INT_TYPE_UART) {
            auto uart = static_cast<UARTIface *>(p);
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
        auto *meteo = static_cast<Meteo *>(c);
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

ConfigsClass Configs;
