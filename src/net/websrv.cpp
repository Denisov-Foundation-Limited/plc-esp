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

void WebServer::indexReq(AsyncWebServerRequest *req)
{
    req->send(200, "text/html", "<h1>FCPLC</h1>");
}

/*********************************************************************/
/*                                                                   */
/*                          PUBLIC FUNCTIONS                         */
/*                                                                   */
/*********************************************************************/

WebServer::WebServer(Logger *log, AsyncWebServer *srv)
{
    _srv = srv;
    _log = log;
}

void WebServer::setEnabled(bool status)
{
    _enabled = status;
}

bool WebServer::getEnabled() const
{
    return _enabled;
}

void WebServer::begin()
{
    if (!_enabled) return;

    _log->info(LOG_MOD_WEB, "Starting web server");

    _srv->on("/", HTTP_GET, std::bind(&WebServer::indexReq, this, std::placeholders::_1));
    _srv->begin();
}
