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

#include "controllers/socket/socket.hpp"
#include "db/socketdb.hpp"
#include "StringUtils.h"
#include "db/eedb.h"

/*********************************************************************/
/*                                                                   */
/*                          PUBLIC FUNCTIONS                         */
/*                                                                   */
/*********************************************************************/

SocketCtrlClass::SocketCtrlClass()
{
    for (size_t i = 0; i < _sockets.size(); i++) {
        memset(&_sockets[i], 0x0, sizeof(Socket));
        _sockets[i].id = i + 1;
    }
}

void SocketCtrlClass::getEnabledSockets(std::vector<Socket *> &socks)
{
    for (size_t i = 0; i < _sockets.size(); i++) {
        if (_sockets[i].enabled) {
            socks.push_back(&_sockets[i]);
        }
    }
}

std::array<Socket, SOCKET_COUNT> *SocketCtrlClass::getSockets()
{
    return &_sockets;
}

bool SocketCtrlClass::setSocket(uint8_t index, Socket *sock)
{
    if (index > (_sockets.size() - 1)) {
        return false;
    }

    memcpy(&_sockets[index], sock, sizeof(Socket));

    return true;
}

bool SocketCtrlClass::isExists(const String &name)
{
    for (size_t i = 0; i < _sockets.size(); i++) {
        if (_sockets[i].name == name) {
            return true;
        }
    }
    return false;
}

bool SocketCtrlClass::getSocket(const String &name, Socket **sock)
{
    for (size_t i = 0; i < _sockets.size(); i++) {
        if (_sockets[i].name == name) {
            *sock = &_sockets[i];
            return true;
        }
    }
    return false;
}

bool SocketCtrlClass::getSocket(size_t index, Socket **sock)
{
    for (size_t i = 0; i < _sockets.size(); i++) {
        if (index == i) {
            *sock = &_sockets[i];
            return true;
        }
    }
    return false;
}

void SocketCtrlClass::begin()
{
    for (size_t i = 0; i < _sockets.size(); i++) {
        if (!_sockets[i].enabled) {
            continue;
        }
        if (_sockets[i].relay != nullptr) {
            Gpio.setMode(_sockets[i].relay, GPIO_MOD_OUTPUT, GPIO_PULL_NONE);
        }
        if (_sockets[i].button != nullptr) {
            Gpio.setMode(_sockets[i].button, GPIO_MOD_INPUT, GPIO_PULL_UP);
        }
    }
    loadStates();
}

void SocketCtrlClass::loop()
{
    if (!_enabled) return;
    if (_sockets.size() == 0) return;

    for (size_t i = 0; i < _sockets.size(); i++) {
        _loopSocket(&_sockets[i]);
    }

    if (_reading) {
        _readButton(&_sockets[_curSocket]);
        if (_curSocket == _sockets.size() - 1) {
            _reading = false;
            _timer = millis();
            _curSocket = 0;
        } else {
            _curSocket++;
        }
    } else {
        if ((millis() - _timer) >= SOCKET_BUTTON_READ_MS) {
            _reading = true;
        }
    }
}

void SocketCtrlClass::setStatus(Socket *sock, bool status, bool save)
{
    sock->status = status;

    Log.info(F("SOCKET"), String(F("Socket ")) + sock->name + String(F(" changed status to ")) + (sock->status ? "ON" : "OFF"));

    if (sock->relay != nullptr) {
        Gpio.write(sock->relay, status);
    }

    if (save) {
        if (EeDb.getEnabled()) {
            EeDbSocket  db;
            if (EeDb.loadSocketDb(db)) {
                if (EeDb.setSocketStatus(db, sock->id, status)) {
                    if (EeDb.saveSocketDb(db)) {
                        Log.info(F("SOCKET"), String(F("Socket status saved to EEPROM. Id: ")) + String(sock->id));
                    } else {
                        Log.error(F("SOCKET"), String(F("Failed to save socket status to EEPROM. Id: ")) + String(sock->id));
                    }
                } else {
                    Log.error(F("SOCKET"), String(F("Failed to set socket status to EEPROM. Id: ")) + String(sock->id));
                }
            } else {
                Log.error(F("SOCKET"), String(F("Failed to load socket status from EEPROM. Id: ")) + String(sock->id));
            }
        } else {
            SocketDB    db;
            db.loadFromFile(F("socket.json"));
            db.close();
            db.setStatus(sock->name, status);
            db.saveToFile();
            db.close();
            db.clear();
        }
    }
}

bool SocketCtrlClass::loadStates()
{
    if (EeDb.getEnabled()) {
        EeDbSocket  db;
        bool        status;

        if (EeDb.loadSocketDb(db)) {
            for (size_t i = 0; i < _sockets.size(); i++) {
                if (!_sockets[i].enabled) {
                    continue;
                }
                if (EeDb.getSocketStatus(db, _sockets[i].id, status)) {
                    Log.info(F("SOCKET"), String(F("Load socket status from EEPROM. Id: ")) + String(_sockets[i].id));
                    setStatus(&_sockets[i], status, false);
                } else {
                    Log.error(F("SOCKET"), String(F("Failed to set socket status to EEPROM. Id: ")) + String(_sockets[i].id));
                }
            }
        } else {
            Log.error(F("SOCKET"), String(F("Failed to load socket DB from EEPROM.")));
        }
    } else {
        SocketDB    db;
        bool        status;

        db.loadFromFile(F("socket.json"));
        if (db.isLoad()) {
            for (size_t i = 0; i < _sockets.size(); i++) {
                if (!_sockets[i].enabled) {
                    continue;
                }
                if (db.getStatus(_sockets[i].name, status)) {
                    setStatus(&_sockets[i], status, false);
                }
            }
            db.clear();
            db.close();
        }
    }
    return true;
}

bool &SocketCtrlClass::getStatus(Socket *sock)
{
    return sock->status;
}

/*********************************************************************/
/*                                                                   */
/*                         PRIVATE FUNCTIONS                         */
/*                                                                   */
/*********************************************************************/

void SocketCtrlClass::_loopSocket(Socket *sock)
{
    if (sock->reading) {
        if ((millis() - sock->timer) >= SOCKET_BUTTON_WAIT_MS) {
            sock->reading = false;
        }
    }
}

void SocketCtrlClass::_readButton(Socket *sock)
{
    if (sock->button != nullptr) {
        if (!Gpio.read(sock->button) && !sock->reading) {
            Log.info(F("SOCKET"), String(F("Socket ")) + sock->name + String(F(" button pressed")));
            setStatus(sock, !getStatus(sock), true);
            sock->reading = true;
            sock->timer = millis();
        }
    }
}

SocketCtrlClass SocketCtrl;
