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

#ifndef __CLI_INFO_HPP__
#define __CLI_INFO_HPP__

#include <Arduino.h>

#include "net/wifi.hpp"
#include "core/gpio.hpp"
#include "core/ext.hpp"

class CLIInformer
{
private:
    Wireless    *_wifi;
    Gpio        *_gpio;
    Extenders   *_ext;
public:
    CLIInformer(Wireless *wifi, Gpio *gpio, Extenders *ext);
    void showWiFi();
    void showWiFiStatus();
    void showTankStatus();
    void showGpio();
    void showGpioStatus();
    void showExtenders();
};

#endif /* __CLI_INFO_HPP__ */
