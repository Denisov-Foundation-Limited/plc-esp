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

UART            gsmSerial;
Logger          logs;
CLIReader       cliReader;
Plc             plc;
TinyGsm         gsmModem(gsmSerial);
Extenders       ext(&logs);
Wireless        wifi(&logs, &plc);
Gpio            gpio(&logs, &ext);
GsmModem        modem(&logs, &gsmSerial, &gsmModem);
Configs         configs(&logs, &modem, &ext, &gpio, &wifi, &plc);
CLIInformer     cliInfo(&wifi, &gpio, &ext);
CLIConfigurator cliCfg(&wifi);
CLIProcessor    cliCp(&logs, &configs, &cliCfg, &cliInfo);

void setup()
{
    logs.begin();
    delay(1000);
    logs.info(LOG_MOD_MAIN, F("Starting controller..."));
    if (!configs.begin()) return;
    plc.begin();
    wifi.begin();
    modem.begin();
    cliCp.begin();
}

void loop()
{
    cliReader.read();
    if (cliReader.isNewString()) {
        cliCp.parse(cliReader.getString());
        cliReader.reset();
    }
    wifi.loop();
    plc.loop();
}
