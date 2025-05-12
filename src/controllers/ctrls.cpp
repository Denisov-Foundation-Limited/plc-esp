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

#include "controllers/ctrls.hpp"
#include "utils/log.hpp"
#include "controllers/socket.hpp"
#include "controllers/meteo.hpp"

/*********************************************************************/
/*                                                                   */
/*                          PUBLIC FUNCTIONS                         */
/*                                                                   */
/*********************************************************************/

void ControllersClass::begin()
{
    SocketCtrl.begin();
    MeteoCtrl.begin();
}

void ControllersClass::loop()
{
    SocketCtrl.loop();
    MeteoCtrl.loop();
}

ControllersClass Controllers;
