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

#include "stack/rpc/rpcsocket.hpp"
#include "stack/rpc/rpcclient.hpp"
#include "controllers/socket.hpp"
#include "stack/stack.hpp"

/*********************************************************************/
/*                                                                   */
/*                          PUBLIC FUNCTIONS                         */
/*                                                                   */
/*********************************************************************/

bool RpcSockets::getSocket(uint8_t unitId, const String &name, std::shared_ptr<RpcSocket> &socket) const
{
    StackUnit       *unit;
    JsonDocument    doc;

    if (unitId == STACK_DEFAULT_UNIT) {
        Socket  *sock;
        if (SocketCtrl.getSocket(name, &sock)) {
            auto s = std::make_shared<RpcSocket>();
            s->name = sock->name;
            s->status = sock->status;
            socket = std::move(s);
            return true;
        }
        return false;
    }

    if (Stack.getUnit(unitId, &unit)) {
        RpcClient client(unit);
        if (client.call(String(F("/socket?name=")) + name, &doc)) {
            auto sock = std::make_shared<RpcSocket>();
            sock->name = doc["name"].as<String>();
            sock->status = doc["status"].as<bool>();
            socket = std::move(sock);
            return true;
        }
    }

    return false;
}

bool RpcSockets::getSockets(uint8_t unitId, std::vector< std::shared_ptr<RpcSocket> > &sockets) const
{
    StackUnit               *unit;
    std::vector<Socket *>   nSockets;
    JsonDocument            doc;

    if (unitId == STACK_DEFAULT_UNIT) {
        SocketCtrl.getSockets(true, nSockets);
        for (auto s : nSockets) {
            auto sock = std::make_shared<RpcSocket>();
            sock->name = s->name;
            sock->status = s->status;
            sockets.push_back(sock);
        }
        return true;
    }

    if (Stack.getUnit(unitId, &unit)) {
        RpcClient client(unit);
        if (client.call(F("/socket"), &doc)) {
            JsonArray socks = doc["sockets"];
            for (auto s : socks) {
                auto sock = std::make_shared<RpcSocket>();
                sock->name = s["name"].as<String>();
                sock->status = s["status"].as<bool>();
                sockets.push_back(sock);
            }
            return true;
        }
    } else {
        Serial.println("FAIL GET UNIT");
    }

    return false;
}

bool RpcSockets::setStatus(uint8_t unitId, const String &name, bool status)
{
    StackUnit       *unit;
    JsonDocument    doc;
    Socket          *socket;

    if (unitId == STACK_DEFAULT_UNIT) {
        if (SocketCtrl.getSocket(name, &socket)) {
            SocketCtrl.setStatus(socket, status, true);
            return true;
        }
        return false;
    }

    if (Stack.getUnit(unitId, &unit)) {
        RpcClient client(unit);
        if (client.call(String(F("/socket?name=")) + name + String(F("&status=")) + String(status), &doc)) {
            if (doc["result"].as<bool>() == true)
                return true;
        }
    }

    return false;
}

bool RpcSockets::toggleStatus(uint8_t unitId, const String &name)
{
    StackUnit       *unit;
    JsonDocument    doc;
    Socket          *socket;

    if (unitId == STACK_DEFAULT_UNIT) {
        if (SocketCtrl.getSocket(name, &socket)) {
            SocketCtrl.setStatus(socket, !socket->status, true);
            return true;
        }
        return false;
    }

    if (Stack.getUnit(unitId, &unit)) {
        RpcClient client(unit);
        if (client.call(String(F("/socket?name=")) + name + String(F("&status=toggle")), &doc)) {
            if (doc["result"].as<bool>() == true)
                return true;
        }
    }

    return false;
}
