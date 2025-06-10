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

#include "controllers/socket.hpp"
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

void SocketCtrlClass::getSockets(bool enabled, std::vector<Socket *> &sockets)
{
    for (size_t i = 0; i < _sockets.size(); i++) {
        if (enabled) {
            if (_sockets[i].enabled) {
                sockets.push_back(&_sockets[i]);
            }
        } else {
            sockets.push_back(&_sockets[i]);
        }
    }
}

bool SocketCtrlClass::setSocket(size_t index, Socket *sock)
{
    if (index > (_sockets.size() - 1)) {
        return false;
    }

    memcpy(&_sockets[index], sock, sizeof(Socket));

    return true;
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

void SocketCtrlClass::begin(bool load)
{
    std::vector<Socket *> sockets;

    getSockets(true, sockets);

    for (size_t i = 0; i < sockets.size(); i++) {
        if (!sockets[i]->enabled) {
            continue;
        }
        if (sockets[i]->relay != nullptr) {
            Gpio.setMode(sockets[i]->relay, GPIO_MOD_OUTPUT, GPIO_PULL_NONE);
        }
        if (sockets[i]->led != nullptr) {
            Gpio.setMode(sockets[i]->led, GPIO_MOD_OUTPUT, GPIO_PULL_NONE);
        }
        if (sockets[i]->button != nullptr) {
            Gpio.setMode(sockets[i]->button, GPIO_MOD_INPUT, GPIO_PULL_UP);
        }
    }

    if (load) {
        _loadStates();
    }
}

void SocketCtrlClass::loop()
{
    if (!_enabled) return;

    std::vector<Socket *> sockets;
    getSockets(true, sockets);

    if (sockets.size() == 0) return;

    if (!_reading) {
        if ((millis() - _timer) >= SOCKET_BUTTON_READ_MS) {
            _reading = true;
            _timer = millis();
        }
    } else {
        _readButton(&_sockets[_curSocket]);

        if (_curSocket < (sockets.size() - 1)) {
            _curSocket++;
        } else {
            _curSocket = 0;
            _reading = false;
            _timer = millis();
        }
    }
}

void SocketCtrlClass::setStatus(Socket *sock, bool status, bool save)
{
    if (sock->status == status)
        return;

    sock->status = status;

    Log.info(F("SOCKET"), String(F("Socket ")) + sock->name + String(F(" changed status to ")) + (sock->status ? "ON" : "OFF"));

    if (sock->relay != nullptr) {
        Gpio.write(sock->relay, status);
    }

    if (sock->led != nullptr) {
        Gpio.write(sock->led, status);
    }

    if (save) {
        if (EeDb.getEnabled()) {
            EeDbSocket  db;
            if (EeDb.loadSocketDb(db)) {
                if (EeDb.setSocketStatus(db, sock->id, status)) {
                    if (EeDb.saveSocketDb(db)) {
                        Log.info(F("SOCKET"), String(F("Socket ")) + sock->name + ((" status saved to EEPROM.")));
                    } else {
                        Log.error(F("SOCKET"), String(F("Socket ")) + sock->name + ((" failed to save status to EEPROM.")));
                    }
                } else {
                    Log.error(F("SOCKET"), String(F("Socket ")) + sock->name + ((" failed to set status to EEPROM.")));
                }
            } else {
                Log.error(F("SOCKET"), String(F("Socket ")) + sock->name + ((" failed to load status from EEPROM.")));
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

bool &SocketCtrlClass::getStatus(Socket *sock)
{
    return sock->status;
}

void SocketCtrlClass::setEnabled(bool enabled)
{
    _enabled = enabled;
}

bool &SocketCtrlClass::getEnabled()
{
    return _enabled;
}


/*********************************************************************/
/*                                                                   */
/*                         PRIVATE FUNCTIONS                         */
/*                                                                   */
/*********************************************************************/

void SocketCtrlClass::_readButton(Socket *sock)
{
    if (sock->button != nullptr) {
        if (!sock->reading) {
            if (Gpio.read(sock->button)) {
                Log.info(F("SOCKET"), String(F("Socket ")) + sock->name + String(F(" button pressed")));
                setStatus(sock, !getStatus(sock), true);
                sock->reading = true;
                sock->timer = millis();
            }
        } else {
            if ((millis() - sock->timer) >= SOCKET_BUTTON_WAIT_MS) {
                sock->reading = false;
            }
        }
    }
}

bool SocketCtrlClass::_loadStates()
{
    std::vector<Socket *> sockets;

    getSockets(true, sockets);

    if (EeDb.getEnabled()) {
        EeDbSocket  db;
        bool        status;

        if (EeDb.loadSocketDb(db)) {
            for (size_t i = 0; i < sockets.size(); i++) {
                if (EeDb.getSocketStatus(db, sockets[i]->id, status)) {
                    Log.info(F("SOCKET"), String(F("Load socket status from EEPROM. Id: ")) + String(sockets[i]->id));
                    setStatus(sockets[i], status, false);
                } else {
                    Log.error(F("SOCKET"), String(F("Failed to set socket status to EEPROM. Id: ")) + String(sockets[i]->id));
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
            for (size_t i = 0; i < sockets.size(); i++) {
                if (db.getStatus(sockets[i]->name, status)) {
                    setStatus(sockets[i], status, false);
                }
            }
            db.clear();
            db.close();
        }
    }
    return true;
}

SocketCtrlClass SocketCtrl;
