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

#ifndef __WEB_SERVER_H__
#define __WEB_SERVER_H__

#include <utils/log.hpp>
#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

#define WEB_SERVER_DEFAULT_PORT 80

class WebServerClass : private AsyncWebServer
{
private:
    bool    _enabled = true;

    void indexReq(AsyncWebServerRequest *req);
public:
    WebServerClass(uint16_t port) : AsyncWebServer(port) {}
    void setEnabled(bool status);
    bool getEnabled() const;
    void begin();
};

extern WebServerClass WebServer;

#endif /* __WEB_SERVER_H__ */
