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

#include "core/cli/clicfg.hpp"
#include "core/ifaces/gpio.hpp"
#include "core/ifaces/ow.hpp"
#include "core/ifaces/spi.hpp"
#include "core/ifaces/i2c.hpp"
#include "core/ifaces/uart.hpp"
#include "net/tgbot.hpp"
#include "controllers/ctrls.hpp"
#include "controllers/meteo/meteo.hpp"
#include "controllers/meteo/sensors/ds18b20.hpp"
#include "net/core/eth.hpp"

/*********************************************************************/
/*                                                                   */
/*                          PUBLIC FUNCTIONS                         */
/*                                                                   */
/*********************************************************************/

bool CLIConfiguratorClass::configWiFi(const String &cmd)
{
    if (cmd == "?" || cmd == "help") {
        Serial.println(F("\nWiFi configurations commands:"));
        Serial.println(F("\tssid <ssid>         : Setup WiFi SSID name"));
        Serial.println(F("\tpasswd <passwd>     : Setup WiFi password"));
        Serial.println(F("\tap <on/off>         : Setup Access Point status"));
        Serial.println(F("\tstatus-led <iface>  : Setup Wi-Fi status LED"));
        Serial.println(F("\tshutdown            : Disable Wi-Fi"));
        Serial.println(F("\tno shutdown         : Enable Wi-Fi"));
        Serial.println(F("\texit                : Exit from WiFi configuration\n"));
        return true;
    }

    if (cmd == "shut" || cmd == "shutdown") {
        Wireless.setEnabled(false);
        return true;
    } else if (cmd == "no shut" || cmd == "no shutdown") {
        Wireless.setEnabled(true);
        Ethernet.setEnabled(false);
        Log.info(LOG_MOD_WIFI, "Ethernet was disabled");
        Wireless.begin();
        return true;
    } else if (cmd.indexOf(F("ssid ")) >= 0) {
        String value(cmd);

        value.remove(0, 5);
        Wireless.setCreds(value, Wireless.getPasswd());

        return true;
    } else if (cmd.indexOf(F("passwd ")) >= 0) {
        String value(cmd);

        value.remove(0, 7);
        Wireless.setCreds(Wireless.getSSID(), value);

        return true;
    } else if (cmd.indexOf(F("ap ")) >= 0) {
        String value(cmd);

        value.remove(0, 3);
        if (value == "on") {
            Wireless.setAP(true);
        } else if (value == "off") {
            Wireless.setAP(false);
        } else {
            return false;
        }

        return true;
    } else if (cmd.indexOf(F("status-led ")) >= 0) {
        String value(cmd);

        value.remove(0, 11);

        auto iface = Interfaces.getInterface(value);

        if (iface == nullptr) {
            Log.error(LOG_MOD_CLI, "Interface not found");
            return true;
        }

        if (iface->getType() != IF_TYPE_GPIO) {
            Log.error(LOG_MOD_CLI, "Interface is not GPIO");
            return true;
        }

        Wireless.setStatusLed(static_cast<IfGPIO *>(iface));

        return true;
    }

    return false;
}

bool CLIConfiguratorClass::configTanks(const String &cmd)
{
    if (cmd == "?" || cmd == "help") {
        Serial.println(F("\nTanks configurations commands:"));
        Serial.println(F("\tadd tank <name(string)> <pump(gpio)> <valve(gpio)> : Add new tank with default params"));
        Serial.println(F("\ttank <name>                                        : Configure tank by name"));
        Serial.println(F("\texit                                               : Exit from Tanks configuration\n"));
        return true;
    }

    if (cmd.indexOf("add tank") >= 0) {
        String value(cmd);
        std::vector<String> params;

        value.remove(0, 9);
        if (!Utils.splitString(value, " ", params))
            return false;

        for (auto p : params) {
            Serial.println("PARAM: " + p);
        }

        if (params.size() != 3)
            return false;

        //Tank tank(params[0], params[1].toInt(), params[2].toInt());
        //TankCtrl.addTank(tank);

        return true;
    }

    return false;
}

bool CLIConfiguratorClass::configTank(const String &tankName, const String &cmd)
{
    if (cmd == "?" || cmd == "help") {
        Serial.println(F("\nTanks configurations commands:"));
        Serial.println(F("\t<name>                                   : new name for tank"));
        Serial.println(F("\tpump <relay>                             : new pump relay for tank"));
        Serial.println(F("\tvalve <relay>                           : new valve relay for tank"));
        Serial.println(F("\tadd level <percent(0-100)> <pin(gpio)> : Add new level ctrl with params"));
        Serial.println(F("\texit                                   : Exit from Tank configuration\n"));
        return true;
    }

    if (cmd.indexOf("add level") >= 0) {
        String value(cmd);
        std::vector<String> params;

        value.remove(0, 9);
        if (!Utils.splitString(value, " ", params))
            return false;
        if (params.size() < 2)
            return false;
        //if (!TankCtrl.isExists(tankName))
        //    return false;

        //Tank &tank = TankCtrl.getTank(tankName);
        //TankLevel level();
        //tank.addLevel(level);

        return true;
    }

    return false;
}

bool CLIConfiguratorClass::configInterfaces(const String &cmd)
{
    if (cmd == "?" || cmd == "help") {
        Serial.println(F("\nInterfaces configurations commands:"));
        Serial.println(F("\tadd if <type(gpio/ow/i2c/spi>) <name(string)> : Add new Interface with type"));
        Serial.println(F("\tif <name>                                     : Configure Interface by name"));
        Serial.println(F("\texit                                          : Exit from interfaces configuration\n"));
        return true;
    }

    if (cmd.indexOf("add if ") >= 0) {
        String              value(cmd);
        std::vector<String> params;

        value.remove(0, 7);

        if (!Utils.splitString(value, " ", params))
            return false;
        if (params.size() < 2)
            return false;

        if (params[0] == "gpio") {
            Interfaces.addInterface(static_cast<Interface *>(new IfGPIO(params[1], GPIO_TYPE_DINPUT, 0, GPIO_MOD_OUTPUT, GPIO_PULL_NONE, EXT_NOT_USED, true)));
        } else if (params[0] == "ow") {
            Interfaces.addInterface(static_cast<Interface *>(new IfOneWire(params[1], 0, true)));
        } else if (params[0] == "i2c") {
            Interfaces.addInterface(static_cast<Interface *>(new IfI2C(params[1], 0, 0, true)));
        } else if (params[0] == "spi") {
            Interfaces.addInterface(static_cast<Interface *>(new IfSPI(params[1], 0, 0, 0, 0, 0, true)));
        } else if (params[0] == "uart") {
            Interfaces.addInterface(static_cast<Interface *>(new IfUART(params[1], 0, 0, 0, true)));
        } else {
            Log.error(LOG_MOD_CLI, "Unknown interface type");
            return true;
        }

        return true;
    }

    return false;
}

bool CLIConfiguratorClass::configInterface(const String &ifaceName, const String &cmd)
{
    auto iface = Interfaces.getInterface(ifaceName);

    if (cmd == "?" || cmd == "help") {
        Serial.println(    F("\nInterface configurations commands:"));
        Serial.println(    F("\tname <string>       : name for Interface"));
        if (iface->getType() == IF_TYPE_GPIO || iface->getType() == IF_TYPE_OW) {
            Serial.println(F("\tpin <integer>       : Setup pin for GPIO/OneWire interface"));
        }
        if (iface->getType() == IF_TYPE_GPIO) {
            Serial.println(F("\ttype <gen/relay/dinput> : Setup type for GPIO interface"));
            Serial.println(F("\tmode <input/output>     : Setup mode for GPIO interface"));
            Serial.println(F("\tpull <none/up/down>     : Setup pull for GPIO interface"));
            Serial.println(F("\text <integer>           : Setup extender for GPIO interface"));
        }
        if (iface->getType() == IF_TYPE_I2C) {
            Serial.println(F("\tsda <integer>       : Setup SDA pin for I2C interface"));
            Serial.println(F("\tscl <integer>       : Setup SCL pin for I2C interface"));
        }
        if (iface->getType() == IF_TYPE_SPI) {
            Serial.println(F("\tmiso <integer>      : Setup MISO pin for SPI interface"));
            Serial.println(F("\tmosi <integer>      : Setup MOSI pin for SPI interface"));
            Serial.println(F("\tsck <integer>       : Setup SCK pin for SPI interface"));
            Serial.println(F("\tss <integer>        : Setup SS pin for SPI interface"));
            Serial.println(F("\tfreq <integer>      : Setup Frequency for SPI interface"));
        }
        if (iface->getType() == IF_TYPE_UART) {
            Serial.println(F("\trx <integer>        : Setup RX pin for UART interface"));
            Serial.println(F("\ttx <integer>        : Setup TX pin for UART interface"));
            Serial.println(F("\tctrl <integer>      : Setup CTRL pin for UART interface"));
            Serial.println(F("\trate <integer>      : Setup Rate for UART interface"));
        }
        Serial.println(    F("\texit                : Exit from Interfaces configurations\n"));
        return true;
    }

    if (cmd.indexOf("name ") >= 0) {
        String  value(cmd);

        value.remove(0, 5);
        iface->setName(value);

        return true;
    }

    switch (iface->getType()) {
        case IF_TYPE_GPIO:
            {
                auto gpio = static_cast<IfGPIO *>(iface);
                if (cmd.indexOf("pin ") >= 0) {
                    String  value(cmd);

                    value.remove(0, 4);
                    gpio->setPin(value.toInt());

                    return true;
                } else if (cmd.indexOf("mode ") >= 0) {
                    String      value(cmd);
                    GpioMode    mode;

                    value.remove(0, 5);

                    if (value == "input") {
                        mode = GPIO_MOD_INPUT;
                    } else if (value == "output") {
                        mode = GPIO_MOD_OUTPUT;
                    } else {
                        return false;
                    }

                    gpio->setMode(mode);

                    return true;
                } else if (cmd.indexOf("type ") >= 0) {
                    String      value(cmd);
                    GpioMode    mode;
                    GpioType    type;

                    value.remove(0, 5);

                    if (value == "gen") {
                        type = GPIO_TYPE_GEN;
                    } else if (value == "relay") {
                        type = GPIO_TYPE_RELAY;
                    } else if (value == "dinput") {
                        type = GPIO_TYPE_DINPUT;
                    } else {
                        return false;
                    }

                    gpio->setPinType(type);

                    return true;
                } else if (cmd.indexOf("pull ") >= 0) {
                    String      value(cmd);
                    GpioPull    pull;

                    value.remove(0, 5);

                    if (value == "none") {
                        pull = GPIO_PULL_NONE;
                    } else if (value == "up") {
                        pull = GPIO_PULL_UP;
                    } else if (value == "down") {
                        pull = GPIO_PULL_DOWN;
                    } else {
                        return false;
                    }

                    gpio->setPull(pull);

                    return true;
                } else if (cmd.indexOf("ext ") >= 0) {
                    String      value(cmd);
                    GpioPull    pull;
                    ExtenderId  ext;

                    value.remove(0, 4);

                    if (value.toInt() > EXT_ID_8 || value.toInt() < EXT_ID_1) {
                        return false;
                    }
                    ext = static_cast<ExtenderId>(value.toInt());

                    if (!Extenders.isExists(ext)) {
                        Serial.println(F("\n\tError: Extender not found\n"));
                        return true;
                    }

                    gpio->setExtId(ext);

                    return true;
                }
            }
            break;

        case IF_TYPE_OW:
            {
                auto ow = static_cast<IfOneWire *>(iface);
                if (cmd.indexOf("pin ") >= 0) {
                    String  value(cmd);

                    value.remove(0, 4);
                    ow->setPin(value.toInt());

                    return true;
                }
            }
            break;

        case IF_TYPE_I2C:
            {
                auto i2c = static_cast<IfI2C *>(iface);
                if (cmd.indexOf("sda ") >= 0) {
                    String  value(cmd);

                    value.remove(0, 4);
                    i2c->setPin(I2C_PIN_SDA, value.toInt());

                    return true;
                } else if (cmd.indexOf("scl ") >= 0) {
                    String  value(cmd);

                    value.remove(0, 4);
                    i2c->setPin(I2C_PIN_SCL, value.toInt());

                    return true;
                }
            }
            break;

        case IF_TYPE_SPI:
            {
                auto spi = static_cast<IfSPI *>(iface);
                if (cmd.indexOf("miso ") >= 0) {
                    String  value(cmd);

                    value.remove(0, 5);
                    spi->setPin(SPI_PIN_MISO, value.toInt());

                    return true;
                } else if (cmd.indexOf("mosi ") >= 0) {
                    String  value(cmd);

                    value.remove(0, 5);
                    spi->setPin(SPI_PIN_MOSI, value.toInt());

                    return true;
                } else if (cmd.indexOf("sck ") >= 0) {
                    String  value(cmd);

                    value.remove(0, 4);
                    spi->setPin(SPI_PIN_SCK, value.toInt());

                    return true;
                } else if (cmd.indexOf("ss ") >= 0) {
                    String  value(cmd);

                    value.remove(0, 3);
                    spi->setPin(SPI_PIN_SS, value.toInt());

                    return true;
                }
            }
            break;

        case IF_TYPE_UART:
            {
                auto uart = static_cast<IfUART *>(iface);
                if (cmd.indexOf("rx ") >= 0) {
                    String  value(cmd);

                    value.remove(0, 3);
                    uart->setPin(UART_PIN_RX, value.toInt());

                    return true;
                } else if (cmd.indexOf("tx ") >= 0) {
                    String  value(cmd);

                    value.remove(0, 3);
                    uart->setPin(UART_PIN_TX, value.toInt());

                    return true;
                } else if (cmd.indexOf("rate ") >= 0) {
                    String  value(cmd);

                    value.remove(0, 5);
                    uart->setRate(value.toInt());

                    return true;
                }
            }
            break;
    }
    return false;
}

bool CLIConfiguratorClass::configExts(const String &cmd)
{
    if (cmd == "?" || cmd == "help") {
        Serial.println(F("\nExtenders configurations commands:"));
        Serial.println(F("\tadd ext <id(1-8)> : Add new GPIO with default params"));
        Serial.println(F("\text <id>          : Configure Extender by id"));
        Serial.println(F("\texit              : Exit from extenders configurations\n"));
        return true;
    }

    if (cmd.indexOf("add ext") >= 0) {
        String      value(cmd);
        unsigned    addr = 0x0;

        value.remove(0, 8);

        if ((addr = Extenders.getLastFreeAddr()) == 0x0) {
            Serial.println(F("\n\tError: No free adresses\n"));
            return true;
        }

        if (value.toInt() > EXT_ID_8 || value.toInt() < EXT_ID_1) {
            return false;
        }

        Extenders.addExtender(new Extender(static_cast<ExtenderId>(value.toInt()), addr, true));

        return true;
    }

    return false;
}

bool CLIConfiguratorClass::configExt(ExtenderId extId, const String &cmd)
{
    if (cmd == "?" || cmd == "help") {
        Serial.println(F("\nExtender configurations commands:"));
        Serial.println(F("\tid <id(1-8)>    : new extender Id"));
        Serial.println(F("\taddr <32-39>    : extender I2C address"));
        Serial.println(F("\texit            : Exit from Extender configurations\n"));
        return true;
    }

    auto ext = Extenders.getById(extId);
    if (ext == nullptr) return false;

    if (cmd.indexOf("id ") >= 0) {
        String  value(cmd);

        value.remove(0, 3);

        if (value.toInt() > EXT_ID_8 || value.toInt() < EXT_ID_1) {
            return false;
        }

        ext->setID(static_cast<ExtenderId>(value.toInt()));

        return true;
    } else if (cmd.indexOf("addr ") >= 0) {
        String  value(cmd);

        value.remove(0, 5);

        if (value.toInt() < EXT_ADDR_1 || value.toInt() > EXT_ADDR_8) {
            return false;
        }

        ext->setAddr(value.toInt());

        return true;
    }

    return false;
}

bool CLIConfiguratorClass::configTgBot(const String &cmd)
{
    if (cmd == "?" || cmd == "help") {
        Serial.println(F("\nTelegram configurations commands:"));
        Serial.println(F("\ttoken <string>          : Setup Telegram API token"));
        Serial.println(F("\tmode <sync/async/long>  : Setup Poll mode"));
        Serial.println(F("\tperiod <millis>         : Setup bot poll period"));
        Serial.println(F("\tproxy-ip <string>       : Setup Proxy IP address"));
        Serial.println(F("\tproxy-port <integer>    : Setup Proxy port"));
        Serial.println(F("\tadd user <string>       : Add telegram bot user"));
        Serial.println(F("\tuser <string>           : Select Telegram bot user for configurations"));
        Serial.println(F("\tshutdown                : Disable Telegram bot"));
        Serial.println(F("\tno shutdown             : Enable Telegram bot"));
        Serial.println(F("\texit                    : Exit from Telegram configuration\n"));
        return true;
    }

    if (cmd == "shutdown" || cmd == "shut") {
        TgBot.setEnabled(false);
        return true;
    } else if (cmd == "no shutdown" || cmd == "no shut") {
        if (!TgBot.getEnabled()) {
            TgBot.setEnabled(true);
            TgBot.begin();
        }
        return true;
    } else if (cmd.indexOf(F("token ")) >= 0) {
        String value(cmd);

        value.remove(0, 6);
        TgBot.setToken(value);

        return true;
    } else if (cmd.indexOf(F("period ")) >= 0) {
        String value(cmd);

        value.remove(0, 7);
        TgBot.setPollMode(TgBot.getPollMode(), value.toInt());

        return true;
    } else if (cmd.indexOf(F("mode ")) >= 0) {
        String  value(cmd);

        value.remove(0, 5);

        if (value == "async") {
            TgBot.setPollMode(fb::Poll::Async, TgBot.getPollPeriod());
        } else if (value == "sync") {
            TgBot.setPollMode(fb::Poll::Sync, TgBot.getPollPeriod());
        } else if (value == "long") {
            TgBot.setPollMode(fb::Poll::Long, TgBot.getPollPeriod());
        } else {
            return false;
        }

        return true;
    } else if (cmd.indexOf(F("proxy-ip ")) >= 0) {
        String value(cmd);

        value.remove(0, 9);
        TgBot.setProxy(value.c_str(), TgBot.getProxyPort());

        return true;
    } else if (cmd.indexOf(F("proxy-port ")) >= 0) {
        String value(cmd);

        value.remove(0, 11);
        TgBot.setProxy(TgBot.getProxyIP(), value.toInt());

        return true;
    } else if (cmd.indexOf(F("add user ")) >= 0) {
        String value(cmd);

        value.remove(0, 9);
        auto user = new TgUser();
        user->name = value;
        TgBot.addUser(user);

        return true;
    }

    return false;
}

bool CLIConfiguratorClass::configTgBotUser(const String &userName, const String &cmd)
{
    if (cmd == "?" || cmd == "help") {
        Serial.println(F("\nTelegram user configurations commands:"));
        Serial.println(F("\tname <string>       : Setup Telegram API key"));
        Serial.println(F("\tadmin <true/false>  : Setup Poll mode"));
        Serial.println(F("\tid <integer>        : Setup Poll mode"));
        Serial.println(F("\tnotify <on/off>     : Setup Poll mode"));
        Serial.println(F("\texit                : Exit from Telegram configuration\n"));
        return true;
    }

    auto *user = TgBot.getUser(userName);
    if (user == nullptr) {
        return false;
    }

    if (cmd.indexOf(F("name ")) >= 0) {
        String value(cmd);

        value.remove(0, 5);
        user->name = value;

        return true;
    } else if (cmd.indexOf(F("admin ")) >= 0) {
        String value(cmd);

        value.remove(0, 6);
        if (value == "true") {
            user->admin = true;
        } else if (value == "false") {
            user->admin = false;
        } else {
            return false;
        }

        return true;
    } else if (cmd.indexOf(F("id ")) >= 0) {
        String value(cmd);

        value.remove(0, 3);
        user->chatId = value.toInt();

        return true;
    } else if (cmd.indexOf(F("notify ")) >= 0) {
        String value(cmd);

        value.remove(0, 7);
        if (value == "on") {
            user->notify = true;
        } else if (value == "off") {
            user->notify = false;
        } else {
            return false;
        }

        return true;
    }

    return false;
}

bool CLIConfiguratorClass::configWebSrv(const String &cmd)
{
    return false;
}

bool CLIConfiguratorClass::configControllers(const String &cmd)
{
    if (cmd == "?" || cmd == "help") {
        Serial.println(F("\nControllers configurations commands:"));
        Serial.println(F("\tctrl <string>           : Select controller for configurations"));
        Serial.println(F("\tadd meteo <string>      : Add Meteo controller with name"));
        Serial.println(F("\tadd security <string>   : Add Security controller with name"));
        Serial.println(F("\tadd socket <string>     : Add Socket controller with name"));
        Serial.println(F("\tadd tank <string>       : Add Tank controller with name"));
        Serial.println(F("\texit                    : Exit from Telegram configuration\n"));
        return true;
    }

    if (cmd.indexOf(F("add meteo ")) >= 0) {
        String value(cmd);

        value.remove(0, 10);
        Controllers.addController(new MeteoCtrl(value));

        return true;
    }

    return false;
}

bool CLIConfiguratorClass::configMeteoCtrl(const String &name, const String &cmd)
{
    if (cmd == "?" || cmd == "help") {
        Serial.println(F("\nMeteo controllers configurations commands:"));
        Serial.println(F("\tname <string>        : Set name for controller"));
        Serial.println(F("\tow <iface>           : Set OneWire bus for controller"));
        Serial.println(F("\tadd ds18b20 <string> : Add DS18B20 meteo sensor by name"));
        Serial.println(F("\tsensor <string>      : Select meteo sensor for configurations"));
        Serial.println(F("\tshutdown             : Disable Meteo controller"));
        Serial.println(F("\tno shutdown          : Enable Meteo controller"));
        Serial.println(F("\texit                 : Exit from Meteo controllers configuration\n"));
        return true;
    }

    auto meteo = static_cast<MeteoCtrl *>(Controllers.getController(name));

    if (cmd == "shutdown" || cmd == "shut") {
        meteo->setEnabled(false);
        return true;
    } else if (cmd == "no shutdown" || cmd == "no shut") {
        if (!meteo->getEnabled()) {
            meteo->setEnabled(true);
            meteo->begin();
        }
        return true;
    } else if (cmd.indexOf(F("name ")) >= 0) {
        String value(cmd);

        value.remove(0, 5);
        meteo->setName(value);

        return true;
    } else if (cmd.indexOf(F("ow ")) >= 0) {
        String value(cmd);

        value.remove(0, 3);
        auto iface = Interfaces.getInterface(value);
        if ((iface == nullptr) || (iface->getType() != IF_TYPE_OW)) {
            return false;
        }
        meteo->setOneWire(static_cast<IfOneWire *>(iface));

        return true;
    } else if (cmd.indexOf(F("add ds18b20 ")) >= 0) {
        String value(cmd);

        value.remove(0, 12);
        meteo->addSensor(new Ds18b20(value));

        return true;
    }

    return false;
}

bool CLIConfiguratorClass::configMeteoSensor(const String &ctrlName, const String &sensName, const String &cmd)
{
    auto meteo = static_cast<MeteoCtrl *>(Controllers.getController(ctrlName));
    auto sensor = meteo->getSensor(sensName);

    if (cmd == "?" || cmd == "help") {
        Serial.println(F("\nMeteo sensor configurations commands:"));
        Serial.println(F("\tname <string>   : Set name for sensor"));
        if (sensor->getType() == METEO_SENSOR_DS18) {
        Serial.println(F("\tid <string>     : Set ID for DS18B20 sensor"));
        }
        Serial.println(F("\tshutdown        : Disable Meteo sensor"));
        Serial.println(F("\tno shutdown     : Enable Meteo sensor"));
        Serial.println(F("\texit            : Exit from Meteo sensors configuration\n"));
        return true;
    }

    if (cmd == "shutdown" || cmd == "shut") {
        sensor->setEnabled(false);
        return true;
    } else if (cmd == "no shutdown" || cmd == "no shut") {
        sensor->setEnabled(true);
        return true;
    } else if (cmd.indexOf(F("name ")) >= 0) {
        String value(cmd);

        value.remove(0, 5);
        sensor->setName(value);

        return true;
    } else if (cmd.indexOf(F("id ")) >= 0) {
        String value(cmd);

        value.remove(0, 3);
        static_cast<Ds18b20 *>(sensor)->setId(value);

        return true;
    }

    return false;
}

CLIConfiguratorClass CLIConfigurator;
