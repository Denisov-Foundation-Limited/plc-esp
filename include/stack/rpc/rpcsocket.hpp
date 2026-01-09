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

#ifndef __RPC_SOCKET_HPP__
#define __RPC_SOCKET_HPP__

#include <Arduino.h>
#include <vector>
#include <memory>

typedef struct {
    String  name;
    bool    status;
} RpcSocket;

class RpcSockets
{
public:
    bool getSocket(uint8_t unitId, const String &name, std::shared_ptr<RpcSocket> &socket) const;
    bool getSockets(uint8_t unitId, std::vector< std::shared_ptr<RpcSocket> > &sockets) const;
    bool setStatus(uint8_t unitId, const String &name, bool status);
    bool toggleStatus(uint8_t unitId, const String &name);
};

#endif /* __RPC_SOCKET_HPP__ */