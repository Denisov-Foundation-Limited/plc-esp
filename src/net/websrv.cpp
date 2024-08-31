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

#include "net/websrv.hpp"

/*********************************************************************/
/*                                                                   */
/*                          PRIVATE FUNCTIONS                         */
/*                                                                   */
/*********************************************************************/

void WebServerClass::indexReq(AsyncWebServerRequest *req)
{
    req->send(200, "text/html", "<h1>FCPLC</h1>");
}

/*********************************************************************/
/*                                                                   */
/*                          PUBLIC FUNCTIONS                         */
/*                                                                   */
/*********************************************************************/

void WebServerClass::setEnabled(bool status)
{
    _enabled = status;
}

bool WebServerClass::getEnabled() const
{
    return _enabled;
}

void WebServerClass::begin()
{
    if (!_enabled) return;

    Log.info(LOG_MOD_WEB, "Starting web server");

    AsyncWebServer::on("/", HTTP_GET, std::bind(&WebServerClass::indexReq, this, std::placeholders::_1));
    AsyncWebServer::begin();
}

WebServerClass WebServer(WEB_SERVER_DEFAULT_PORT);
