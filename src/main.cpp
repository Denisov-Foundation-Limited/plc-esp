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
#include "net/core/gsm.hpp"
#include "net/core/wifi.hpp"
#include "core/plc.hpp"
#include "core/cli/clicfg.hpp"
#include "core/cli/cliinfo.hpp"
#include "core/cli/clicp.hpp"
#include "net/tgbot.hpp"
#include "net/apiserver.hpp"
#include "controllers/ctrls.hpp"
#include "net/webgui.hpp"
#include "net/core/eth.hpp"

void setup()
{
    Log.begin();
    delay(1000);
    Serial.println("");
    Log.info(LOG_MOD_MAIN, F("Starting controller..."));
    if (!Configs.begin()) return;
    Plc.begin();
    Wireless.begin();
    Ethernet.begin();
    TgBot.begin();
    GsmModem.begin();
    APIServer.begin();
    Controllers.begin();
    CLIProcessor.begin();
    WebGUI.begin();
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
    Controllers.loop();
    WebGUI.loop();
}
