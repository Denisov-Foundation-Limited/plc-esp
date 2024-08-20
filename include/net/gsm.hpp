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

#include "core/gpio.hpp"
#include "utils/log.hpp"

using namespace EspSoftwareSerial;

typedef enum {
    GSM_GPIO_RX,
    GSM_GPIO_TX,
    GSM_GPIO_RST,
    GSM_GPIO_MAX
} GsmGpio;

class GsmModem
{
private:
    bool    _enabled = false;
    GpioPin *_pins[GSM_GPIO_MAX];
    unsigned _rate;

    Logger  *_log;
    UART    *_gsmUart;
    TinyGsm *_modem;

    String getRegStatus(const SIM800RegStatus state) const;
    String getSigLevel(int level) const;
public:
    GsmModem(Logger *log, UART *gsmUart, TinyGsm *modem);
    void setGpio(GsmGpio gpio, GpioPin *pin);
    GpioPin *getGpio(GsmGpio gpio) const;
    void setRate(unsigned rate);
    unsigned getRate() const;
    void setEnabled(bool status);
    bool getEnabled() const;
    void begin();
};

#endif /* __GSM_HPP__ */
