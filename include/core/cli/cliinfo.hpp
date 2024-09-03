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
#include "core/ifaces/ifaces.hpp"
#include "core/ext.hpp"

class CLIInformerClass
{
public:
    void showWiFi();
    void showWiFiStatus();
    void showTankStatus();
    void showInterfaces();
    void showInterfacesStatus();
    void showExtenders();
    void showControllers();
    void showMeteo();
    void showMeteoStatus();
    void showOneWire();
    void showI2C();
};

extern CLIInformerClass CLIInformer;

#endif /* __CLI_INFO_HPP__ */
