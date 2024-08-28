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

class WebServer
{
private:
    bool    _enabled = true;

    AsyncWebServer  *_srv;
    Logger          *_log;

    void indexReq(AsyncWebServerRequest *req);
public:
    WebServer(Logger *log, AsyncWebServer *srv);
    void setEnabled(bool status);
    bool getEnabled() const;
    void begin();
};

#endif /* __WEB_SERVER_H__ */
