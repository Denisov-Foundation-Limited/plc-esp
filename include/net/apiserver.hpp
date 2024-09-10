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

#include <utils/log.hpp>
#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

#define API_SERVER_DEFAULT_PORT 8080

class APIServerClass : private AsyncWebServer
{
private:
    bool    _enabled = true;

    void indexReq(AsyncWebServerRequest *req);
public:
    APIServerClass(uint16_t port) : AsyncWebServer(port) {}
    void setEnabled(bool status);
    bool getEnabled() const;
    void begin();
};

extern APIServerClass APIServer;

#endif /* __API_SERVER_HPP__ */
