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

#ifndef __API_SERVER_HPP__
#define __API_SERVER_HPP__

#include "utils/log.hpp"
#include "controllers/socket/socket.hpp"

#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>

#define API_SERVER_DEFAULT_PORT 8080

class APIServerClass : private AsyncWebServer
{
public:
    APIServerClass(uint16_t port) : AsyncWebServer(port) {}
    void setEnabled(bool status);
    bool getEnabled() const;
    void begin();

private:
    bool    _enabled = true;
    void    _socketHandler(SocketCtrl *ctrl, AsyncWebServerRequest *req, JsonDocument *out);
    void    _sendError(JsonDocument *out, const String &msg);
};

extern APIServerClass APIServer;

#endif /* __API_SERVER_HPP__ */
