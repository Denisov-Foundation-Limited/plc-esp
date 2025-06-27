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

#ifndef __API_SERVER_HPP__
#define __API_SERVER_HPP__

#include "utils/log.hpp"
#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>

#include "net/api/handlers/socketh.hpp"

#define API_SERVER_PORT 8000

class APIServerClass
{
public:
    void setEnabled(bool status);
    bool getEnabled() const;
    void begin();

private:
    bool            _enabled = true;
    SocketHandler   _socket;
};

extern APIServerClass APIServer;
extern AsyncWebServer WebServer;

#endif /* __API_SERVER_HPP__ */
