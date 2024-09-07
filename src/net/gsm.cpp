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
/*                          PUBLIC FUNCTIONS                         */
/*                                                                   */
/*********************************************************************/

GsmModemClass::GsmModemClass()
{
    _modem = new TinyGsm(_gsmUart);
}

void GsmModemClass::setUart(UARTIface *uart)
{
    _uart = uart;
}

void GsmModemClass::setEnabled(bool status)
{
    _enabled = status;
}

bool GsmModemClass::getEnabled() const
{
    return _enabled;
}

UARTIface *GsmModemClass::getUart() const
{
    return _uart;
}

void GsmModemClass::begin()
{
    if (!_enabled) return;

    Log.info(LOG_MOD_GSM, F("Starting GSM modem"));
    _gsmUart.begin(_uart->getRate(), SWSERIAL_8N1, _uart->getPin(UART_PIN_RX), _uart->getPin(UART_PIN_TX));
    _modem->restart();

    Log.info(LOG_MOD_GSM, String(F("Modem Info: ")) + _modem->getModemInfo());

    if (_modem->getSimStatus() != SIM_READY) {
        Log.error(LOG_MOD_GSM, F("SIM not ready"));
        return;
    } else {
        Log.info(LOG_MOD_GSM, F("SIM is ready"));
    }
    
    if (!_modem->waitForNetwork()) 
    {
        Log.error(LOG_MOD_GSM, F("Failed to connect to network"));
        return;
    }
    else
    {
        auto regStatus = _modem->getRegistrationStatus();
        Log.info(LOG_MOD_GSM, String(F("Registration : ")) + getRegStatus(regStatus));
        Log.info(LOG_MOD_GSM, String(F("CCID         : ")) + _modem->getSimCCID());
        Log.info(LOG_MOD_GSM, String(F("IMEI         : ")) + _modem->getIMEI());
        Log.info(LOG_MOD_GSM, String(F("Operator     : ")) + _modem->getOperator());
        Log.info(LOG_MOD_GSM, String(F("Signal       : ")) + getSigLevel(_modem->getSignalQuality()));
    }
}

/*********************************************************************/
/*                                                                   */
/*                          PRIVATE FUNCTIONS                        */
/*                                                                   */
/*********************************************************************/

String GsmModemClass::getRegStatus(const SIM800RegStatus state) const
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

String GsmModemClass::getSigLevel(int level) const
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

GsmModemClass GsmModem;
