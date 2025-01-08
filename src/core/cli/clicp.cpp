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

#include "core/cli/clicp.hpp"
#include "net/tgbot.hpp"
#include "controllers/ctrls.hpp"
#include "controllers/meteo/meteo.hpp"
#include "ftest.hpp"

/*********************************************************************/
/*                                                                   */
/*                          PUBLIC FUNCTIONS                         */
/*                                                                   */
/*********************************************************************/

void CLIProcessorClass::begin()
{
    Serial.printf("\n\n%s", F("console#"));
}

bool CLIProcessorClass::parse(const String &cmd)
{
    if (cmd == "exit" || cmd == "ex") {
        _processExit();
    } else if (cmd == "") {
    } else {
        bool isOk = false;

        switch (_level)
        {
        case CON_LEVEL_WIFI:
            isOk = CLIConfigurator.configWiFi(cmd);
            break;

        case CON_LEVEL_TG:
            isOk = CLIConfigurator.configTgBot(cmd);
            if (!isOk && (cmd.indexOf(F("user ")) >= 0)) {
                String  value(cmd);
                TgUser  *user;

                value.remove(0, 5);

                if (!TgBot.getUser(value, &user)) {
                    _objName = value;
                    _level = CON_LEVEL_TG_USR;
                } else {
                    Log.error(F("CLI"), String(F("TgBot user ")) + value + String(F(" not found.")));
                }
                isOk = true;
            }
            break;

        case CON_LEVEL_TG_USR:
            isOk = CLIConfigurator.configTgBotUser(_objName, cmd);
            break;

        case CON_LEVEL_ENABLE:
            isOk = _parseEnableCmd(cmd);
            break;

        case CON_LEVEL_CONFIG:
            isOk = _parseConfigCmd(cmd);
            break;
        };

        if (!isOk) {
            if (cmd.indexOf(F("do show ")) >= 0 ||
                cmd == "do reload" ||
                cmd == "do reset" ||
                cmd == "do write" ||
                cmd == "do erase")
            {
                String enCmd(cmd);
                enCmd.remove(0, 3);
                isOk = _parseEnableCmd(enCmd);
            }
            if (!isOk) {
                Serial.printf("\n\tUnrecognized command: \"%s\"\n\n", cmd.c_str());
            }
        }
    }

    _printCall();
    return true;
}

bool CLIProcessorClass::_parseEnableCmd(const String &cmd)
{
    if (cmd == "reset" || cmd == "reload") {
        ESP.restart();
    } else if ((cmd == "show int") || (cmd == "show interfaces")) {
        CLIInformer.showInterfaces();
    } else if ((cmd == "show int status") || (cmd == "show interfaces status")) {
        CLIInformer.showInterfacesStatus();
    } else if (cmd == "show ext") {
        CLIInformer.showExtenders();
    } else if (cmd == "show wifi") {
        CLIInformer.showWiFi();
    } else if (cmd == "show ow") {
        CLIInformer.showOneWire();
    } else if (cmd == "show tgbot") {
        CLIInformer.showTgBot();
    } else if (cmd == "show i2c") {
        CLIInformer.showI2C();
    } else if (cmd == "show meteo status") {
        CLIInformer.showMeteoStatus();
    } else if (cmd == "ftest") {
        Ftest.start();
    } else if ((cmd == "show start") || (cmd == "show startup")) {
        if (!Configs.showStartup()) {
            Log.error(F("CLI"), F("Startup configs not found"));
        }
    } else if ((cmd == "show run") || (cmd == "show running")) {
        Configs.showRunning();
    } else if (cmd == "show wifi status") {
        CLIInformer.showWiFiStatus();
    } else if (cmd == "write") {
        if (Configs.writeAll()) {
            Log.info(F("CLI"), F("Configs was saved"));
        } else {
            Log.error(F("CLI"), F("Failed to save configs"));
        }
    } else if (cmd == "erase") {
        if (Configs.eraseAll()) {
            Log.info(F("CLI"), F("Configs was erased"));
        } else {
            Log.error(F("CLI"), F("Failed to erase configs"));
        }
    } else if (cmd == "config" || cmd == "con") {
        _level = CON_LEVEL_CONFIG;
    } else if (cmd == "?" || cmd == "help") {
        Serial.println(F("\nDevice console commands:"));
        Serial.println(F("\tshow wifi               : WiFi configurations"));
        Serial.println(F("\tshow wifi status        : WiFi connection status"));
        Serial.println(F("\tshow ctrl               : Controllers configurations"));
        Serial.println(F("\tshow interfaces         : Interfaces configurations"));
        Serial.println(F("\tshow interfaces status  : Interfaces statuses"));
        Serial.println(F("\tshow tgbot              : Telegram bot configurations"));
        Serial.println(F("\tshow meteo              : Meteo controller configurations"));
        Serial.println(F("\tshow meteo status       : Meteo controller sensors status"));
        Serial.println(F("\tshow ext                : I2C extenders configurations"));
        Serial.println(F("\tshow ow                 : Print OneWire devices on bus"));
        Serial.println(F("\tshow i2c                : Print I2C devices on bus"));
        Serial.println(F("\tshow startup            : Print configs saved to flash"));
        Serial.println(F("\tshow running            : Print configs from RAM"));
        Serial.println(F("\treload                  : Reboot device"));
        Serial.println(F("\twrite                   : Save all configurations to flash"));
        Serial.println(F("\terase                   : Erase configurations and load default\n"));
    } else {
        return false;
    }

    return true;
}

bool CLIProcessorClass::_parseConfigCmd(const String &cmd)
{
    if (cmd == "wifi") {
        _level = CON_LEVEL_WIFI;
    } else if (cmd == "tanks") {
        _level = CON_LEVEL_TANKS;
    } else if (cmd == "tgbot") {
        _level = CON_LEVEL_TG;
    } else if (cmd == "web") {
        _level = CON_LEVEL_WEB;
    } else if (cmd == "ext") {
        _level = CON_LEVEL_EXTS;
    } else if (cmd == "ctrl") {
        _level = CON_LEVEL_CTRLS;
    } else if (cmd == "interfaces" || cmd == "int") {
        _level = CON_LEVEL_IFACES;
    } else if (cmd == "?" || cmd == "help") {
        Serial.println(F("\nDevice configuration console commands:"));
        Serial.println(F("\twifi        : WiFi module configurations"));
        Serial.println(F("\tinterfaces  : Interfaces configurations"));
        Serial.println(F("\ttgbot       : Telegram bot configurations"));
        Serial.println(F("\tweb         : Web server configurations"));
        Serial.println(F("\tctrl        : Controllers configurations"));
        Serial.println(F("\text         : Extenders configurations"));
        Serial.println(F("\texit        : Exit from configuration\n"));
    } else {
        return false;
    }
    return true;
}

void CLIProcessorClass::_processExit()
{
    switch (_level)
    {
    case CON_LEVEL_WIFI:
    case CON_LEVEL_TANKS:
    case CON_LEVEL_IFACES:
    case CON_LEVEL_TG:
    case CON_LEVEL_WEB:
    case CON_LEVEL_CTRLS:
    case CON_LEVEL_EXTS:
        _level = CON_LEVEL_CONFIG;
        break;

    case CON_LEVEL_TANK:
        _level = CON_LEVEL_TANKS;
        break;

    case CON_LEVEL_TG_USR:
        _level = CON_LEVEL_TG;
        break;

    case CON_LEVEL_CONFIG:
        _level = CON_LEVEL_ENABLE;
        break;

    case CON_LEVEL_IFACE:
        _level = CON_LEVEL_IFACES;
        break;

    case CON_LEVEL_EXT:
        _level = CON_LEVEL_EXTS;
        break;

    case CON_LEVEL_METEO:
        _level = CON_LEVEL_CTRLS;
        break;

    case CON_LEVEL_METEO_SENS:
        _level = CON_LEVEL_METEO;
        break;

    case CON_LEVEL_ENABLE:
        break;
    }
}

/*********************************************************************/
/*                                                                   */
/*                          PRIVATE FUNCTIONS                        */
/*                                                                   */
/*********************************************************************/

void CLIProcessorClass::_printCall()
{
    switch (_level)
    {
    case CON_LEVEL_ENABLE:
        Serial.printf("%s", F("console#"));
        break;

    case CON_LEVEL_CONFIG:
        Serial.printf("%s", F("(config)#"));
        break;

    case CON_LEVEL_WIFI:
        Serial.printf("%s", F("config(wifi)#"));
        break;

    case CON_LEVEL_TANKS:
        Serial.printf("%s", F("config(tanks)#"));
        break;

    case CON_LEVEL_TANK:
        Serial.printf("%s", F("config(tanks-tnk)#"));
        break;

    case CON_LEVEL_IFACES:
        Serial.printf("%s", F("config(ifaces)#"));
        break;

    case CON_LEVEL_IFACE:
        Serial.printf("%s", F("config(ifaces-if)#"));
        break;

    case CON_LEVEL_EXTS:
        Serial.printf("%s", F("config(exts)#"));
        break;

    case CON_LEVEL_EXT:
        Serial.printf("%s", F("config(exts-ext)#"));
        break;

    case CON_LEVEL_TG:
        Serial.printf("%s", F("config(tgbot)#"));
        break;

    case CON_LEVEL_TG_USR:
        Serial.printf("%s", F("config(tgbot-usr)#"));
        break;

    case CON_LEVEL_WEB:
        Serial.printf("%s", F("config(web)#"));
        break;

    case CON_LEVEL_CTRLS:
        Serial.printf("%s", F("config(ctrls)#"));
        break;

    case CON_LEVEL_METEO:
        Serial.printf("%s", F("config(meteo)#"));
        break;

    case CON_LEVEL_METEO_SENS:
        Serial.printf("%s", F("config(meteo-sens)#"));
        break;
    }
}

CLIProcessorClass CLIProcessor;
