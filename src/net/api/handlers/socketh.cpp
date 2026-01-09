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
        RpcSocket       *socket = nullptr;
        uint8_t         unit = RPC_DEFAULT_UNIT;

        if (req->hasArg(F("unit"))) {
            unit = req->arg(F("unit")).toInt();
        }

        if (!req->hasArg(F("name"))) {
            _showSocketAll(unit, &jOut);
        } else {
            if (req->hasArg(F("status"))) {
                _setSocketStatus(unit, req->arg(F("name")), req->arg(F("status")), &jOut);
            } else {
                _showSocket(unit, req->arg(F("name")), &jOut);
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

void SocketHandler::_showSocketAll(uint8_t unitId, JsonDocument *out)
{
    std::vector< std::shared_ptr<RpcSocket> > sockets;

    if (Rpc.getSocket()->getSockets(unitId, sockets)) {
        for (size_t i = 0; i < sockets.size(); i++) {
            (*out)[F("sockets")][i][F("name")] = sockets[i]->name;
            (*out)[F("sockets")][i][F("status")] = sockets[i]->status;
        }
        (*out)["result"] = true;
        return;
    }

    (*out)["result"] = false;
}

void SocketHandler::_showSocket(uint8_t unitId, const String &name, JsonDocument *out)
{
    std::shared_ptr<RpcSocket> socket;

    if (Rpc.getSocket()->getSocket(unitId, name, socket)) {
        (*out)["name"] = socket->name;
        (*out)["status"] = socket->status;
        (*out)["result"] = true;
        return;
    }

    (*out)["error"] = F("Socket not found");
    (*out)["result"] = false;
}

bool SocketHandler::_setSocketStatus(uint8_t unitId, const String &name, const String &status, JsonDocument *out)
{
    if (status == F("true")) {
        if (!Rpc.getSocket()->setStatus(unitId, name, true)) {
            (*out)["result"] = false;
            return false;
        }
    } else if (status == F("false")) {
        if (!Rpc.getSocket()->setStatus(unitId, name, false)) {
            (*out)["result"] = false;
            return false;
        }
    } else if (status == F("toggle")) {
        if (!Rpc.getSocket()->toggleStatus(unitId, name)) {
            (*out)["result"] = false;
            return false;
        }
    } else {
        (*out)["result"] = false;
        (*out)["error"] = F("Unknown socket status");
        return false;
    }

    (*out)["result"] = true;
    return true;
}
