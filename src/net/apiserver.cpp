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

#include "net/apiserver.hpp"
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

    Log.info(F("API"), F("Starting API server at :8080"));

    AsyncWebServer::on("/", HTTP_GET, [this](AsyncWebServerRequest *req) {
    });

    AsyncWebServer::on("/socket", HTTP_GET, [this](AsyncWebServerRequest *req) {
        JsonDocument    jOut;
        String          sOut;
        Socket          *socket = nullptr;

        if (req->getParam(F("name")) == nullptr) {
            _socketHandler(nullptr, req, &jOut);
        } else {
            if (!SocketCtrl.getSocket(req->getParam(F("name"))->value(), &socket)) {
                jOut["result"] = false;
                jOut["error"] = F("Socket not found");
            } else {
                _socketHandler(socket, req, &jOut);
            }
        }

        serializeJson(jOut, sOut);
        req->send(200, "application/json", sOut);
    });

    AsyncWebServer::begin();
}

/*********************************************************************/
/*                                                                   */
/*                          PRIVATE FUNCTIONS                         */
/*                                                                   */
/*********************************************************************/

void APIServerClass::_socketHandler(Socket *sock, AsyncWebServerRequest *req, JsonDocument *out)
{
    if (req->getParam(F("socket")) != nullptr) {
        if (sock != nullptr) {
            if (req->getParam(F("status")) != nullptr) {
                if (req->getParam(F("status"))->value() == "true") {
                    SocketCtrl.setStatus(sock, true, true);
                } else if (req->getParam(F("status"))->value() == "false") {
                    SocketCtrl.setStatus(sock, false, true);
                } else if (req->getParam(F("status"))->value() == "switch") {
                    SocketCtrl.setStatus(sock, !sock->status, true);
                } else {
                    _sendError(out, F("Unknown socket status"));
                    return;
                }
            } else {
                (*out)[F("name")] = sock->name;
                (*out)[F("status")] = sock->status;
            }
        } else {
            _sendError(out, F("Socket not found"));
            return;
        }
        (*out)["result"] = true;
        return;
    }

    std::vector<Socket *> socks;
    SocketCtrl.getEnabledSockets(socks);

    for (size_t i = 0; i < socks.size(); i++) {
        (*out)[F("sockets")][i][F("name")] = socks[i]->name;
        (*out)[F("sockets")][i][F("status")] = socks[i]->status;
    }
    (*out)["result"] = true;
}

void APIServerClass::_sendError(JsonDocument *out, const String &msg)
{
    (*out)[F("result")] = false;
    (*out)[F("error")] = msg;
    Log.error(F("API"), msg);
}

APIServerClass APIServer(API_SERVER_DEFAULT_PORT);
