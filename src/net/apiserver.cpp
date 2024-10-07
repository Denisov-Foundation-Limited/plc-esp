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

    Log.info(LOG_MOD_API, F("Starting API server at :8080"));

    AsyncWebServer::on("/", HTTP_GET, [this](AsyncWebServerRequest *req) {
    });

    AsyncWebServer::on("/ctrl", HTTP_GET, [this](AsyncWebServerRequest *req) {
        JsonDocument    jOut;
        String          sOut;

        auto name = req->getParam(F("name"));
        auto ctrl = Controllers.getController(name->value());

        if (ctrl == nullptr) {
            jOut["result"] = false;
            jOut["error"] = F("Controller not found");
        } else {
            switch (ctrl->getType()) {
                case CTRL_TYPE_SOCKET:
                    _socketHandler(static_cast<SocketCtrl *>(ctrl), req, &jOut);
                    break;
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

void APIServerClass::_socketHandler(SocketCtrl *ctrl, AsyncWebServerRequest *req, JsonDocument *out)
{
    if (req->getParam(F("socket")) != nullptr) {
        auto socket = ctrl->getSocket(req->getParam(F("socket"))->value());
        if (socket != nullptr) {
            if (req->getParam(F("status")) != nullptr) {
                if (req->getParam(F("status"))->value() == "true") {
                    socket->setStatus(true, true);
                } else if (req->getParam(F("status"))->value() == "false") {
                    socket->setStatus(false, true);
                } else if (req->getParam(F("status"))->value() == "switch") {
                    socket->setStatus(!socket->getStatus(), true);
                } else {
                    _sendError(out, F("Unknown socket status"));
                    return;
                }
            } else {
                (*out)[F("name")] = socket->getName();
                (*out)[F("status")] = socket->getStatus();
            }
        } else {
            _sendError(out, F("Socket not found"));
            return;
        }
        (*out)["result"] = true;
        return;
    }

    for (size_t i = 0; i < ctrl->getSockets().size(); i++) {
        auto s = ctrl->getSockets()[i];
        (*out)[F("sockets")][i][F("name")] = s->getName();
        (*out)[F("sockets")][i][F("status")] = s->getStatus();
    }
    (*out)["result"] = true;
}

void APIServerClass::_sendError(JsonDocument *out, const String &msg)
{
    (*out)[F("result")] = false;
    (*out)[F("error")] = msg;
    Log.error(LOG_MOD_API, msg);
}

APIServerClass APIServer(API_SERVER_DEFAULT_PORT);
