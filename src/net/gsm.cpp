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

#include "net/gsm.hpp"

/*********************************************************************/
/*                                                                   */
/*                          PRIVATE FUNCTIONS                        */
/*                                                                   */
/*********************************************************************/

String GsmModem::getRegStatus(const SIM800RegStatus state) const
{
    switch (state)
    {
        case REG_UNREGISTERED:
            return F("Not registered, MT is not currently searching");
        case REG_SEARCHING:
            return F("Not registered, MT is not currently searching");
        case REG_DENIED:
            return F("Registration denied");
        case REG_OK_HOME:
            return F("Registered, home network");
        case REG_OK_ROAMING:
            return F("Registered, roaming");
        case REG_UNKNOWN:
        case REG_NO_RESULT:
            return F("Unknown");
    }
    return F("Unknown");
}

String GsmModem::getSigLevel(int level) const
{
    switch (level)
    {
        case 0:
            return F("-115 dBm or less");
        case 1:
            return F("-111 dBm");
        case 31:
            return F("-52 dBm or greater");
        case 99:
            return F("not known or not detectable");
        default:   
            if (level > 1 && level < 31)
            return F("-110... -54 dBm");
    }
    return F("Unknown");
}

/*********************************************************************/
/*                                                                   */
/*                          PUBLIC FUNCTIONS                         */
/*                                                                   */
/*********************************************************************/

GsmModem::GsmModem(Logger *log, UART *gsmUart, TinyGsm *modem)
{
    _gsmUart = gsmUart;
    _modem = modem;
    _log = log;
}

void GsmModem::setUart(UARTIface *uart)
{
    _uart = uart;
}

void GsmModem::setEnabled(bool status)
{
    _enabled = status;
}

bool GsmModem::getEnabled() const
{
    return _enabled;
}

UARTIface *GsmModem::getUart() const
{
    return _uart;
}

void GsmModem::begin()
{
    if (!_enabled) return;

    _log->info(LOG_MOD_GSM, F("Starting GSM modem"));
    _gsmUart->begin(_uart->getRate(), SWSERIAL_8N1, _uart->getPin(UART_PIN_RX), _uart->getPin(UART_PIN_TX));
    _modem->restart();

    _log->info(LOG_MOD_GSM, String(F("Modem Info: ")) + _modem->getModemInfo());

    if (_modem->getSimStatus() != SIM_READY) {
        _log->error(LOG_MOD_GSM, F("SIM not ready"));
        return;
    } else {
        _log->info(LOG_MOD_GSM, F("SIM is ready"));
    }
    
    if (!_modem->waitForNetwork()) 
    {
        _log->error(LOG_MOD_GSM, F("Failed to connect to network"));
        return;
    }
    else
    {
        auto regStatus = _modem->getRegistrationStatus();
        _log->info(LOG_MOD_GSM, String(F("Registration : ")) + getRegStatus(regStatus));
        _log->info(LOG_MOD_GSM, String(F("CCID         : ")) + _modem->getSimCCID());
        _log->info(LOG_MOD_GSM, String(F("IMEI         : ")) + _modem->getIMEI());
        _log->info(LOG_MOD_GSM, String(F("Operator     : ")) + _modem->getOperator());
        _log->info(LOG_MOD_GSM, String(F("Signal       : ")) + getSigLevel(_modem->getSignalQuality()));
    }
}