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

#include "net/api/apiserver.hpp"
#include "controllers/ctrls.hpp"

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

    Log.info(F("API"), String(F("Starting API server at :")) + String(API_SERVER_PORT));

    WebServer.on("/", HTTP_GET, [](AsyncWebServerRequest *req) {
        JsonDocument    jOut;
        String          sOut;

        jOut["result"] = true;

        serializeJson(jOut, sOut);
        req->send(200, "application/json", sOut);
    });

    _socket.registerHandler(&WebServer);

    WebServer.begin();
}

/*********************************************************************/
/*                                                                   */
/*                          PRIVATE FUNCTIONS                        */
/*                                                                   */
/*********************************************************************/

APIServerClass APIServer;
AsyncWebServer WebServer(API_SERVER_PORT);
