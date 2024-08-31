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

#ifndef __GSM_HPP__
#define __GSM_HPP__

#define TINY_GSM_MODEM_SIM800

#include <TinyGSM.h>
#include <SoftwareSerial.h>

#include "core/ifaces/uart.hpp"
#include "utils/log.hpp"

using namespace EspSoftwareSerial;

class GsmModem
{
private:
    bool        _enabled = false;
    UARTIface   *_uart;

    Logger  *_log;
    UART    *_gsmUart;
    TinyGsm *_modem;

    String getRegStatus(const SIM800RegStatus state) const;
    String getSigLevel(int level) const;
public:
    GsmModem(Logger *log, UART *gsmUart, TinyGsm *modem);
    void setUart(UARTIface *uart);
    UARTIface *getUart() const;
    void setEnabled(bool status);
    bool getEnabled() const;
    void begin();
};

#endif /* __GSM_HPP__ */
