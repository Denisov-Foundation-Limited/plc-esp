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

CLIConfigurator::CLIConfigurator(Logger *log, Wireless *wifi, Gpio *gpio, Extenders *ext)
{
    _wifi = wifi;
    _gpio = gpio;
    _ext = ext;
    _log = log;
}

bool CLIConfigurator::configWiFi(const String &cmd)
{
    if (cmd == "?" || cmd == "help") {
        Serial.println(F("\nWiFi configurations commands:"));
        Serial.println(F("\tset enabled <true/false> : Setup enabled mode for WiFi"));
        Serial.println(F("\tset ssid <ssid>          : Setup WiFi SSID name"));
        Serial.println(F("\tset passwd <passwd>      : Setup WiFi password"));
        Serial.println(F("\tset ap <on/off>          : Setup Access Point status"));
        Serial.println(F("\texit                     : Exit from WiFi configuration\n"));
        return true;
    }

    if (cmd.indexOf(F("set enabled")) >= 0) {
        String value(cmd);

        value.remove(0, 12);
        if (value == "true") {
            _wifi->setEnabled(true);
        } else if (value == "false") {
            _wifi->setEnabled(false);
        } else {
            return false;
        }

        return true;
    } else if (cmd.indexOf(F("set ssid")) >= 0) {
        String value(cmd);

        value.remove(0, 9);
        _wifi->setCreds(value, _wifi->getPasswd());

        return true;
    } else if (cmd.indexOf(F("set passwd")) >= 0) {
        String value(cmd);

        value.remove(0, 11);
        _wifi->setCreds(_wifi->getSSID(), value);

        return true;
    } else if (cmd.indexOf(F("set ap")) >= 0) {
        String value(cmd);

        value.remove(0, 7);
        if (value == "on") {
            _wifi->setAP(true);
        } else if (value == "off") {
            _wifi->setAP(false);
        } else {
            return false;
        }

        return true;
    }

    return false;
}

bool CLIConfigurator::configTanks(const String &cmd)
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
        if (!splitString(value, " ", params))
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

bool CLIConfigurator::configTank(const String &tankName, const String &cmd)
{
    if (cmd == "?" || cmd == "help") {
        Serial.println(F("\nTanks configurations commands:"));
        Serial.println(F("\tset <name>                             : Set new name for tank"));
        Serial.println(F("\tset pump <relay>                       : Set new pump relay for tank"));
        Serial.println(F("\tset valve <relay>                      : Set new valve relay for tank"));
        Serial.println(F("\tadd level <percent(0-100)> <pin(gpio)> : Add new level ctrl with params"));
        Serial.println(F("\texit                                   : Exit from Tank configuration\n"));
        return true;
    }

    if (cmd.indexOf("add level") >= 0) {
        String value(cmd);
        std::vector<String> params;

        value.remove(0, 9);
        if (!splitString(value, " ", params))
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

bool CLIConfigurator::configGpio(const String &cmd)
{
    if (cmd == "?" || cmd == "help") {
        Serial.println(F("\nGPIO configurations commands:"));
        Serial.println(F("\tadd pin <name(string)> : Add new GPIO with default params"));
        Serial.println(F("\tpin <name>             : Configure GPIO pin by name"));
        Serial.println(F("\texit                   : Exit from Tanks configuration\n"));
        return true;
    }

    if (cmd.indexOf("add pin") >= 0) {
        String value(cmd);

        value.remove(0, 8);
        _gpio->addGpio(new GpioPin(_log, _ext, value, 0, GPIO_TYPE_INPUT, GPIO_PULL_NONE, EXT_NOT_USED));

        return true;
    }

    return false;
}

bool CLIConfigurator::configPin(const String &pinName, const String &cmd)
{
    if (cmd == "?" || cmd == "help") {
        Serial.println(F("\nPin configurations commands:"));
        Serial.println(F("\tset name <string>       : Set new name for GPIO pin"));
        Serial.println(F("\tset pin <integer>       : Configure GPIO pin by name"));
        Serial.println(F("\tset type <input/output> : Configure GPIO pin by name"));
        Serial.println(F("\tset pull <none/up/down> : Configure GPIO pin by name"));
        Serial.println(F("\tset ext <integer>       : Configure GPIO pin by name"));
        Serial.println(F("\texit                    : Exit from GPIO configurations\n"));
        return true;
    }

    auto pin = _gpio->getPin(pinName);
    if (pin == nullptr) return false;

    if (cmd.indexOf("set name") >= 0) {
        String              value(cmd);

        value.remove(0, 9);
        pin->setName(value);

        return true;
    } else if (cmd.indexOf("set pin") >= 0) {
        String  value(cmd);

        value.remove(0, 8);
        

        pin->setPin(value.toInt());

        return true;
    } else if (cmd.indexOf("set type") >= 0) {
        String      value(cmd);
        GpioType    type;

        value.remove(0, 9);

        if (value == "input") {
            type = GPIO_TYPE_INPUT;
        } else if (value == "output") {
            type = GPIO_TYPE_OUTPUT;
        } else {
            return false;
        }

        pin->setType(type);

        return true;
    } else if (cmd.indexOf("set pull") >= 0) {
        String      value(cmd);
        GpioPull    pull;

        value.remove(0, 9);

        if (value == "none") {
            pull = GPIO_PULL_NONE;
        } else if (value == "up") {
            pull = GPIO_PULL_UP;
        } else if (value == "down") {
            pull = GPIO_PULL_DOWN;
        } else {
            return false;
        }

        pin->setPull(pull);

        return true;
    } else if (cmd.indexOf("set ext") >= 0) {
        String      value(cmd);
        GpioPull    pull;
        ExtenderId  ext;

        value.remove(0, 8);

        if (value.toInt() > EXT_ID_8 || value.toInt() < EXT_ID_1) {
            return false;
        }
        ext = static_cast<ExtenderId>(value.toInt());

        if (!_ext->isExists(ext)) {
            Serial.println(F("\n\tError: Extender not found\n"));
            return true;
        }

        pin->setExtId(ext);

        return true;
    }

    return false;
}

bool CLIConfigurator::configExts(const String &cmd)
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

        if ((addr = _ext->getLastFreeAddr()) == 0x0) {
            Serial.println(F("\n\tError: No free adresses\n"));
            return true;
        }

        if (value.toInt() > EXT_ID_8 || value.toInt() < EXT_ID_1) {
            return false;
        }

        _ext->addExtender(new Extender(static_cast<ExtenderId>(value.toInt()), addr));

        return true;
    }

    return false;
}

bool CLIConfigurator::configExt(ExtenderId extId, const String &cmd)
{
    if (cmd == "?" || cmd == "help") {
        Serial.println(F("\nExtender configurations commands:"));
        Serial.println(F("\tset id <id(1-8)> : Set new extender Id"));
        Serial.println(F("\tset addr <32-39> : Set extender I2C address"));
        Serial.println(F("\texit             : Exit from Extender configurations\n"));
        return true;
    }

    auto ext = _ext->getById(extId);
    if (ext == nullptr) return false;

    if (cmd.indexOf("set id") >= 0) {
        String  value(cmd);

        value.remove(0, 7);

        if (value.toInt() > EXT_ID_8 || value.toInt() < EXT_ID_1) {
            return false;
        }

        ext->setID(static_cast<ExtenderId>(value.toInt()));

        return true;
    } else if (cmd.indexOf("set addr") >= 0) {
        String  value(cmd);

        value.remove(0, 9);

        if (value.toInt() < EXT_ADDR_1 || value.toInt() > EXT_ADDR_8) {
            return false;
        }

        ext->setAddr(value.toInt());

        return true;
    }

    return false;
}
