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

#ifndef __SOCKET_HANDLER_HPP__
#define __SOCKET_HANDLER_HPP__

#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include "controllers/socket.hpp"

class SocketHandler
{
public:
    void registerHandler(AsyncWebServer *server);

private:
    void _socketShow(Socket *socket, AsyncWebServerRequest *req, JsonDocument *out);
    bool _socketStatus(Socket *socket, AsyncWebServerRequest *req, JsonDocument *out);
};

#endif /* __SOCKET_HANDLER_HPP__ */
