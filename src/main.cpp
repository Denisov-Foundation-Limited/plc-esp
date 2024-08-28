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
#include "core/gpio.hpp"
#include "core/cli/clireader.hpp"
#include "net/gsm.hpp"
#include "net/wifi.hpp"
#include "core/plc.hpp"
#include "core/cli/clicfg.hpp"
#include "core/cli/cliinfo.hpp"
#include "core/cli/clicp.hpp"
#include "net/tgbot.hpp"
#include "net/websrv.hpp"

auto gsmSerial = new UART();
auto logs = new Logger();
auto cliReader = new CLIReader();
auto plc = new Plc();
auto fb2 = new FastBot2();
auto aSrv = new AsyncWebServer(80);
auto gsmModem = new TinyGsm(*gsmSerial);
auto ext = new Extenders(logs);
auto webSrv = new WebServer(logs, aSrv);
auto tgBot = new TgBot(logs, fb2);
auto wifi = new Wireless(logs, plc);
auto gpio = new Gpio(logs, ext);
auto modem = new GsmModem(logs, gsmSerial, gsmModem);
auto configs = new Configs(logs, modem, ext, gpio, wifi, plc);
auto cliInfo = new CLIInformer(wifi, gpio, ext);
auto cliCfg = new CLIConfigurator(logs, wifi, gpio, ext);
auto cliCp = new CLIProcessor(logs, configs, cliCfg, cliInfo, gpio, ext);

void setup()
{
    logs->begin();
    delay(1000);
    Serial.println("");
    logs->info(LOG_MOD_MAIN, F("Starting controller..."));
    if (!configs->begin()) return;
    plc->begin();
    wifi->begin();
    tgBot->begin();
    modem->begin();
    webSrv->begin();
    cliCp->begin();
}

void loop()
{
    cliReader->read();
    if (cliReader->isNewString()) {
        cliCp->parse(cliReader->getString());
        cliReader->reset();
    }
    wifi->loop();
    plc->loop();
    tgBot->loop();
}
