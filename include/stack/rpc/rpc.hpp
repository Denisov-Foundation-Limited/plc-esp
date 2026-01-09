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

#ifndef __RPC_HPP__
#define __RPC_HPP__

#include "rpcmeteo.hpp"
#include "rpcsocket.hpp"
#include "stack/stack.hpp"

#define RPC_DEFAULT_UNIT    STACK_DEFAULT_UNIT

class RpcClass
{
public:
    RpcMeteo *getMeteo() { return &_meteo; }
    RpcSockets *getSocket() { return &_socket; }

private:
    RpcMeteo    _meteo;
    RpcSockets  _socket;
};

extern RpcClass Rpc;

#endif /* __RPC_HPP__ */