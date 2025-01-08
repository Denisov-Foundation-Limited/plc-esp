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

#ifndef __GSM_HPP__
#define __GSM_HPP__

#define TINY_GSM_MODEM_SIM800

#include <TinyGSM.h>
#include <SoftwareSerial.h>

#include "core/ifaces/uart.hpp"
#include "utils/log.hpp"

using namespace EspSoftwareSerial;

class GsmModemClass
{
private:
    bool        _enabled = false;
    UARTClass   *_uart;

    UART        _gsmUart;
    TinyGsm     *_modem;

    String getRegStatus(const SIM800RegStatus state) const;
    String getSigLevel(int level) const;
public:
    GsmModemClass();
    void setUart(UARTClass *uart);
    UARTClass *getUart() const;
    void setEnabled(bool status);
    bool getEnabled() const;
    void begin();
};

extern GsmModemClass GsmModem;

#endif /* __GSM_HPP__ */
