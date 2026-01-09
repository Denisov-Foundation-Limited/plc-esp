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

#ifndef __RPC_CLIENT_HPP__
#define __RPC_CLIENT_HPP__

#include <Arduino.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

#include "stack/stack.hpp"

#define RPC_CLIENT_TIMEOUT_MS  500

class RpcClient
{
public:
    RpcClient(StackUnit *unit);
    bool call(const String &request, JsonDocument *out);

private:
    HTTPClient  _client;
    StackUnit   *_unit;
};

#endif /* __RPC_CLIENT_HPP__ */
