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

#include "net/api/handlers/socketh.hpp"

/*********************************************************************/
/*                                                                   */
/*                          PUBLIC FUNCTIONS                         */
/*                                                                   */
/*********************************************************************/

void SocketHandler::registerHandler(AsyncWebServer *server)
{
    server->on("/socket", HTTP_GET, [this](AsyncWebServerRequest *req) {
        JsonDocument    jOut;
        String          sOut;
        Socket          *socket = nullptr;

        if (req->getParam(F("name")) == nullptr) {
            _socketShow(nullptr, req, &jOut);
        } else {
            if (SocketCtrl.getSocket(req->getParam(F("name"))->value(), &socket)) {
                if (req->getParam(F("status")) != nullptr) {
                    if (!_socketStatus(socket, req, &jOut)) {
                        serializeJson(jOut, sOut);
                        req->send(403, F("application/json"), sOut);
                        return;
                    }
                } else {
                    _socketShow(socket, req, &jOut);
                }
            } else {
                jOut["result"] = false;
                jOut["error"] = F("Socket not found");
                serializeJson(jOut, sOut);
                req->send(404, F("application/json"), sOut);
                return;
            }
        }

        serializeJson(jOut, sOut);
        req->send(200, F("application/json"), sOut);
    });
}

/*********************************************************************/
/*                                                                   */
/*                          PRIVATE FUNCTIONS                        */
/*                                                                   */
/*********************************************************************/

void SocketHandler::_socketShow(Socket *socket, AsyncWebServerRequest *req, JsonDocument *out)
{
    std::vector<Socket *> socks;
    SocketCtrl.getSockets(true, socks);

    for (size_t i = 0; i < socks.size(); i++) {
        (*out)[F("sockets")][i][F("name")] = socks[i]->name;
        (*out)[F("sockets")][i][F("status")] = socks[i]->status;
    }
    (*out)["result"] = true;
}

bool SocketHandler::_socketStatus(Socket *socket, AsyncWebServerRequest *req, JsonDocument *out)
{
    if (req->getParam(F("status")) != nullptr) {
        if (req->getParam(F("status"))->value() == "true") {
            SocketCtrl.setStatus(socket, true, true);
        } else if (req->getParam(F("status"))->value() == "false") {
            SocketCtrl.setStatus(socket, false, true);
        } else if (req->getParam(F("status"))->value() == "toggle") {
            SocketCtrl.setStatus(socket, !socket->status, true);
        } else {
            (*out)["result"] = false;
            (*out)["error"] = F("Unknown socket status");
            return false;
        }
    }
    (*out)["result"] = true;
    return true;
}
