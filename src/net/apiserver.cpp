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

#include "net/apiserver.hpp"

/*********************************************************************/
/*                                                                   */
/*                          PUBLIC FUNCTIONS                         */
/*                                                                   */
/*********************************************************************/

void APIServerClass::setEnabled(bool status)
{
    _enabled = status;
}

bool APIServerClass::getEnabled() const
{
    return _enabled;
}

void APIServerClass::begin()
{
    if (!_enabled) return;

    Log.info(LOG_MOD_WEB, F("Starting API server at :8080"));

    AsyncWebServer::on("/", HTTP_GET, std::bind(&APIServerClass::indexReq, this, std::placeholders::_1));
    AsyncWebServer::begin();
}

/*********************************************************************/
/*                                                                   */
/*                          PRIVATE FUNCTIONS                         */
/*                                                                   */
/*********************************************************************/

void APIServerClass::indexReq(AsyncWebServerRequest *req)
{
    req->send(200, "text/html", "<h1>FCPLC</h1>");
}

APIServerClass APIServer(API_SERVER_DEFAULT_PORT);
