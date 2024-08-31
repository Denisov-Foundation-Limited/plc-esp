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

#include "utils/log.hpp"
#include "utils/configs.hpp"
#include "core/ext.hpp"
#include "core/ifaces/ifaces.hpp"
#include "core/cli/clireader.hpp"
#include "net/gsm.hpp"
#include "net/wifi.hpp"
#include "core/plc.hpp"
#include "core/cli/clicfg.hpp"
#include "core/cli/cliinfo.hpp"
#include "core/cli/clicp.hpp"
#include "net/tgbot.hpp"
#include "net/websrv.hpp"

void setup()
{
    Log.begin();
    delay(1000);
    Serial.println("");
    Log.info(LOG_MOD_MAIN, F("Starting controller..."));
    if (!Configs.begin()) return;
    Plc.begin();
    Wireless.begin();
    TgBot.begin();
    GsmModem.begin();
    WebServer.begin();
    CLIProcessor.begin();
}

void loop()
{
    CLIReader.read();
    if (CLIReader.isNewString()) {
        CLIProcessor.parse(CLIReader.getString());
        CLIReader.reset();
    }
    Wireless.loop();
    Plc.loop();
    TgBot.loop();
}
