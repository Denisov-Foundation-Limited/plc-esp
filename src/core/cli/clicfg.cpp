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

CLIConfigurator::CLIConfigurator(Wireless *wifi)
{
    _wifi = wifi;
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
        Serial.println(F("\tadd tank <name(string)> <pump(relay)> <valve(relay)> : Add new tank with default params"));
        Serial.println(F("\ttank <name>                                          : Configure tank by name"));
        Serial.println(F("\texit                                                 : Exit from Tanks configuration\n"));
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
        Serial.println(F("\tset <name>                              : Set new name for tank"));
        Serial.println(F("\tset pump <relay>                        : Set new pump relay for tank"));
        Serial.println(F("\tset valve <relay>                       : Set new valve relay for tank"));
        Serial.println(F("\tadd level <percent(0-100)> <pin(input)> : Add new level ctrl with params"));
        Serial.println(F("\texit                                    : Exit from Tank configuration\n"));
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
