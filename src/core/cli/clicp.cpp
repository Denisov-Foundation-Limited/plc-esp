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

#include "core/cli/clicp.hpp"

/*********************************************************************/
/*                                                                   */
/*                          PRIVATE FUNCTIONS                        */
/*                                                                   */
/*********************************************************************/

void CLIProcessor::printCall()
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
    }
}

/*********************************************************************/
/*                                                                   */
/*                          PUBLIC FUNCTIONS                         */
/*                                                                   */
/*********************************************************************/

CLIProcessor::CLIProcessor(Logger  *log, Configs *cfg, CLIConfigurator *cliCfg, CLIInformer *cliInfo, Interfaces *ifaces, Extenders *ext)
{
    _log = log;
    _cfg = cfg;
    _cliCfg = cliCfg;
    _cliInfo = cliInfo;
    _ifaces = ifaces;
    _ext = ext;
}

void CLIProcessor::begin()
{
    Serial.printf("\n\n%s", F("console#"));
}

bool CLIProcessor::parse(const String &cmd)
{
    if (cmd == "exit" || cmd == "ex") {
        processExit();
    } else if (cmd == "") {
    } else {
        bool isOk = false;

        switch (_level)
        {
        case CON_LEVEL_WIFI:
            isOk = _cliCfg->configWiFi(cmd);
            break;

        case CON_LEVEL_TANKS:
            isOk = _cliCfg->configTanks(cmd);
            if (!isOk && (cmd.indexOf("tank ") >= 0)) {
                String value(cmd);

                value.remove(0, 5);
                /*if (_tankCtrl.isExists(value)) {
                    _objName = value;
                    _level = CON_LEVEL_TANK;
                } else {
                    _log->error(LOG_MOD_CLI, "Tank " + value + " not found.");
                }*/
                isOk = true;
            }
            break;

        case CON_LEVEL_TANK:
            isOk = _cliCfg->configTank(_objName, cmd);
            break;

        case CON_LEVEL_IFACES:
            isOk = _cliCfg->configInterfaces(cmd);
            if (!isOk && (cmd.indexOf("if ") >= 0)) {
                String value(cmd);

                value.remove(0, 3);

                if (_ifaces->getInterface(value) != nullptr) {
                    _objName = value;
                    _level = CON_LEVEL_IFACE;
                } else {
                    _log->error(LOG_MOD_CLI, String(F("Interface ")) + value + String(F(" not found.")));
                }
                isOk = true;
            }
            break;

        case CON_LEVEL_IFACE:
            isOk = _cliCfg->configInterface(_objName, cmd);
            break;

        case CON_LEVEL_EXTS:
            isOk = _cliCfg->configExts(cmd);
            if (!isOk && (cmd.indexOf("ext ") >= 0)) {
                String value(cmd);

                value.remove(0, 4);

                if (_ext->getById(static_cast<ExtenderId>(value.toInt())) != nullptr) {
                    _objName = value;
                    _level = CON_LEVEL_EXT;
                } else {
                    _log->error(LOG_MOD_CLI, String(F("Extender ")) + value + String(F(" not found.")));
                }
                isOk = true;
            }
            break;

        case CON_LEVEL_EXT:
            isOk = _cliCfg->configExt(static_cast<ExtenderId>(_objName.toInt()), cmd);
            break;

        case CON_LEVEL_ENABLE:
            isOk = parseEnableCmd(cmd);
            break;

        case CON_LEVEL_CONFIG:
            isOk = parseConfigCmd(cmd);
            break;
        };

        if (!isOk) {
            if (cmd.indexOf("do show ") >= 0 ||
                cmd == "do reload" ||
                cmd == "do reset" ||
                cmd == "do write" ||
                cmd == "do erase")
            {
                String enCmd(cmd);
                enCmd.remove(0, 3);
                isOk = parseEnableCmd(enCmd);
            }
            if (!isOk) {
                Serial.printf("\n\tUnrecognized command: \"%s\"\n\n", cmd.c_str());
            }
        }
    }

    printCall();
    return true;
}

bool CLIProcessor::parseEnableCmd(const String &cmd)
{
    if (cmd == "reset" || cmd == "reload") {
        ESP.restart();
    } else if ((cmd == "show int") || (cmd == "show interfaces")) {
        _cliInfo->showInterfaces();
    } else if ((cmd == "show int status") || (cmd == "show interfaces status")) {
        _cliInfo->showInterfacesStatus();
    } else if (cmd == "show ext") {
        _cliInfo->showExtenders();
    } else if (cmd == "show wifi") {
        _cliInfo->showWiFi();
    } else if ((cmd == "show start") || (cmd == "show startup")) {
        if (!_cfg->showStartup()) {
            _log->error(LOG_MOD_CLI, F("Startup configs not found"));
        }
    } else if ((cmd == "show run") || (cmd == "show running")) {
        _cfg->showRunning();
    } else if (cmd == "show wifi status") {
        _cliInfo->showWiFiStatus();
    } else if (cmd == "show tank status") {
        _cliInfo->showTankStatus();
    } else if (cmd == "write") {
        if (_cfg->writeAll()) {
            _log->info(LOG_MOD_CLI, F("Configs was saved"));
        } else {
            _log->error(LOG_MOD_CLI, F("Failed to save configs"));
        }
    } else if (cmd == "erase") {
        if (_cfg->eraseAll()) {
            _log->info(LOG_MOD_CLI, F("Configs was erased"));
        } else {
            _log->error(LOG_MOD_CLI, F("Failed to erase configs"));
        }
    } else if (cmd == "config" || cmd == "con") {
        _level = CON_LEVEL_CONFIG;
    } else if (cmd == "?" || cmd == "help") {
        Serial.println(F("\nDevice console commands:"));
        Serial.println(F("\tshow wifi               : WiFi configurations"));
        Serial.println(F("\tshow wifi status        : WiFi connection status"));
        Serial.println(F("\tshow interfaces         : Interfaces configurations"));
        Serial.println(F("\tshow interfaces status  : Interfaces statuses"));
        Serial.println(F("\tshow ext                : I2C extenders configurations"));
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

bool CLIProcessor::parseConfigCmd(const String &cmd)
{
    if (cmd == "wifi") {
        _level = CON_LEVEL_WIFI;
    } else if (cmd == "tanks") {
        _level = CON_LEVEL_TANKS;
    } else if (cmd == "ext") {
        _level = CON_LEVEL_EXTS;
    } else if (cmd == "interfaces" || cmd == "int") {
        _level = CON_LEVEL_IFACES;
    } else if (cmd == "?" || cmd == "help") {
        Serial.println(F("\nDevice configuration console commands:"));
        Serial.println(F("\twifi        : WiFi module configurations"));
        Serial.println(F("\tinterfaces  : Interfaces configurations"));
        Serial.println(F("\text         : Extenders configurations"));
        Serial.println(F("\texit        : Exit from configuration\n"));
    } else {
        return false;
    }
    return true;
}

void CLIProcessor::processExit()
{
    switch (_level)
    {
    case CON_LEVEL_WIFI:
    case CON_LEVEL_TANKS:
    case CON_LEVEL_IFACES:
    case CON_LEVEL_EXTS:
        _level = CON_LEVEL_CONFIG;
        break;

    case CON_LEVEL_TANK:
        _level = CON_LEVEL_TANKS;
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

    case CON_LEVEL_ENABLE:
        break;
    }
}
