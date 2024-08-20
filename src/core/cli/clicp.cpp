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
        Serial.printf("%s", "console#");
        break;

    case CON_LEVEL_CONFIG:
        Serial.printf("%s", "(config)#");
        break;

    case CON_LEVEL_WIFI:
        Serial.printf("%s", "config(wifi)#");
        break;

    case CON_LEVEL_TANKS:
        Serial.printf("%s", "config(tanks)#");
        break;

    case CON_LEVEL_TANK:
        Serial.printf("%s", "config(tanks-tnk)#");
        break;
    }
}

/*********************************************************************/
/*                                                                   */
/*                          PUBLIC FUNCTIONS                         */
/*                                                                   */
/*********************************************************************/

CLIProcessor::CLIProcessor(Logger  *log, Configs *cfg, CLIConfigurator *cliCfg, CLIInformer *cliInfo)
{
    _log = log;
    _cfg = cfg;
    _cliCfg = cliCfg;
    _cliInfo = cliInfo;
}

void CLIProcessor::begin()
{
    Serial.printf("\n\n%s", "console#");
}

bool CLIProcessor::parse(const String &cmd)
{
    if (cmd == "exit") {
        switch (_level)
        {
        case CON_LEVEL_WIFI:
        case CON_LEVEL_TANKS:
        case CON_LEVEL_TANK:
            _level = CON_LEVEL_CONFIG;
            break;

        case CON_LEVEL_CONFIG:
            _level = CON_LEVEL_ENABLE;
            break;

        case CON_LEVEL_ENABLE:
            break;
        }
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

        case CON_LEVEL_ENABLE:
            if (cmd == "reset" || cmd == "reload") {
                ESP.restart();
                isOk = true;
            } else if (cmd == "show gpio") {
                _cliInfo->showGpio();
                isOk = true;
            } else if (cmd == "show gpio status") {
                _cliInfo->showGpioStatus();
                isOk = true;
            } else if (cmd == "show ext") {
                _cliInfo->showExtenders();
                isOk = true;
            } else if (cmd == "show wifi") {
                _cliInfo->showWiFi();
                isOk = true;
            } else if (cmd == "show wifi status") {
                _cliInfo->showWiFiStatus();
                isOk = true;
            } else if (cmd == "show tank status") {
                _cliInfo->showTankStatus();
                isOk = true;
            } else if (cmd == "write") {
                if (_cfg->writeAll()) {
                    _log->info(LOG_MOD_CLI, F("Configs was saved"));
                } else {
                    _log->error(LOG_MOD_CLI, F("Failed to save configs"));
                }
                isOk = true;
            } else if (cmd == "erase") {
                _cfg->eraseAll();
                isOk = true;
            } else if (cmd == "config") {
                _level = CON_LEVEL_CONFIG;
                isOk = true;
            } else if (cmd == "?" || cmd == "help") {
                Serial.println(F("\nDevice console commands:"));
                Serial.println(F("\tshow wifi        : WiFi configurations"));
                Serial.println(F("\tshow wifi status : WiFi connection status"));
                Serial.println(F("\tshow gpio        : GPIO configurations"));
                Serial.println(F("\tshow gpio status : GPIO statuses"));
                Serial.println(F("\tshow ext         : I2C extenders configurations"));
                Serial.println(F("\treload           : Reboot device"));
                Serial.println(F("\twrite            : Save all configurations to flash"));
                Serial.println(F("\terase            : Erase configurations and load default\n"));
                isOk = true;
            }
            break;

        case CON_LEVEL_CONFIG:
            if (cmd == "wifi") {
                    _level = CON_LEVEL_WIFI;
                    isOk = true;
            } else if (cmd == "tanks") {
                    _level = CON_LEVEL_TANKS;
                    isOk = true;
            } else if (cmd == "?" || cmd == "help") {
                Serial.println(F("\nDevice configuration console commands:"));
                Serial.println(F("\twifi : WiFi module configurations"));
                Serial.println(F("\texit : Exit from configuration\n"));
                isOk = true;
            }
            break;
        };

        if (!isOk) {
            Serial.printf("\n\tUnrecognized command: \"%s\"\n\n", cmd.c_str());
        }
    }

    printCall();
    return true;
}
