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

#include "stack/rpc/rpcclient.hpp"
#include "net/api/apiserver.hpp"

/*********************************************************************/
/*                                                                   */
/*                          PUBLIC FUNCTIONS                         */
/*                                                                   */
/*********************************************************************/

RpcClient::RpcClient(StackUnit *unit)
{
    _unit = unit;
    _client.setTimeout(RPC_CLIENT_TIMEOUT_MS);
    _client.setConnectTimeout(RPC_CLIENT_TIMEOUT_MS);
}

bool RpcClient::call(const String &request, JsonDocument *out)
{
    if (_unit == nullptr)
        return false;

    if (_client.begin("http://" + _unit->ip + ":" + String(API_SERVER_PORT) + request)) {
        int code = _client.GET();
        if (code == 200) {
            if (deserializeJson(*out, _client.getString()) == DeserializationError::Ok) {
                _client.end();
                return true;
            }
        }
    }
    _client.end();
    return false;
}
