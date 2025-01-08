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

#ifndef __CLI_INFO_HPP__
#define __CLI_INFO_HPP__

#include <Arduino.h>

#include "net/core/wifi.hpp"
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
    void showTgBot();
};

extern CLIInformerClass CLIInformer;

#endif /* __CLI_INFO_HPP__ */
