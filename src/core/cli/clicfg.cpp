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
        Log.info(F("CLI"), F("Ethernet was disabled"));
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
    }

    return false;
}

bool CLIConfiguratorClass::configTgBotUser(const String &userName, const String &cmd)
{
    TgUser  *user;

    if (cmd == "?" || cmd == "help") {
        Serial.println(F("\nTelegram user configurations commands:"));
        Serial.println(F("\tname <string>       : Setup Telegram API key"));
        Serial.println(F("\tadmin <true/false>  : Setup Poll mode"));
        Serial.println(F("\tid <integer>        : Setup Poll mode"));
        Serial.println(F("\tnotify <on/off>     : Setup Poll mode"));
        Serial.println(F("\texit                : Exit from Telegram configuration\n"));
        return true;
    }

    if (!TgBot.getUser(userName, &user)) {
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

CLIConfiguratorClass CLIConfigurator;
