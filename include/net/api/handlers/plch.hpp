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

#ifndef __PLC_HANDLER_HPP__
#define __PLC_HANDLER_HPP__

#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>

class PlcHandler
{
public:
    void registerHandler(AsyncWebServer *server);
};

#endif /* __PLC_HANDLER_HPP__ */
