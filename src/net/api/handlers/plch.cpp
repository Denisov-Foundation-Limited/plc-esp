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

#include "net/api/handlers/plch.hpp"
#include "core/plc.hpp"

/*********************************************************************/
/*                                                                   */
/*                          PUBLIC FUNCTIONS                         */
/*                                                                   */
/*********************************************************************/

void PlcHandler::registerHandler(AsyncWebServer *server)
{
    server->on("/plc", HTTP_GET, [this](AsyncWebServerRequest *req) {
        JsonDocument    jOut;
        String          sOut;

        jOut[F("name")] = Plc.getName();
        jOut[F("temp")] = static_cast<int>(Plc.getBoardTemp());
        jOut[F("fan")] = Plc.getFanStatus();
        jOut[F("alarm")] = Plc.getAlarm();
        jOut[F("status")] = Plc.getStatus();

        serializeJson(jOut, sOut);
        req->send(200, F("application/json"), sOut);
    });
}
