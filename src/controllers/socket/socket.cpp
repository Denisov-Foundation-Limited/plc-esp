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

#include "controllers/socket/socket.hpp"

Socket::Socket(const String &name)
{
    _name = name;
}

void Socket::setStatus(bool status, bool save)
{
    _status = status;
}

bool Socket::getStatus() const
{
    return _status;
}

void Socket::setName(const String &name)
{
    _name = name;
}

const String &Socket::getName() const
{
    return _name;
}

void Socket::begin()
{
    if (_gpio[SOCK_IF_RELAY] != nullptr) {
        _gpio[SOCK_IF_RELAY]->setMode(GPIO_MOD_OUTPUT);
        _gpio[SOCK_IF_RELAY]->write(false);
    }
    if (_gpio[SOCK_IF_BUTTON] != nullptr) {
        _gpio[SOCK_IF_BUTTON]->setMode(GPIO_MOD_INPUT);
    }
}

void Socket::loop()
{
    if (_reading) {
        if ((millis() - _timer) >= SOCKET_BUTTON_WAIT_MS) {
            _reading = false;
        }
    }
}

void Socket::readButton()
{
    if (_gpio[SOCK_IF_BUTTON] != nullptr) {
        if (_gpio[SOCK_IF_BUTTON]->read() && !_reading) {
            setStatus(!getStatus(), true);
            _reading = true;
            _timer = millis();
        }
    }
}

void Socket::setInterface(SockIfType type, Interface *iface)
{
    _gpio[type] = static_cast<IfGPIO *>(iface);
}

Interface *Socket::getInterface(SockIfType type) const
{
    return _gpio[type];
}

SocketCtrl::SocketCtrl(const String &name)
{
    _name = name;
}

void SocketCtrl::addSocket(Socket *sock)
{
    Log.info(LOG_MOD_SOCKET, String(F("Add socket: ")) + sock->getName());
    _sockets.push_back(sock);
}

const std::vector<Socket *> &SocketCtrl::getSockets()
{
    return _sockets;
}

bool SocketCtrl::isExists(const String &name)
{
    for (auto sock : _sockets) {
        if (sock->getName() == name) {
            return true;
        }
    }
    return false;
}

Socket *SocketCtrl::getSocket(const String &name)
{
    for (auto sock : _sockets) {
        if (sock->getName() == name) {
            return sock;
        }
    }
    return nullptr;
}

void SocketCtrl::begin()
{
    if (!_enabled) return;
    for (auto s : _sockets) {
        s->begin();
    }
}

void SocketCtrl::loop()
{
    if (!_enabled) return;
    if (_sockets.size() == 0) return;

    for (auto s : _sockets) {
        s->loop();
    }

    if (_reading) {
        _sockets[_curSocket]->readButton();
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

bool SocketCtrl::getEnabled() const
{
    return _enabled;
}

void SocketCtrl::setEnabled(bool enabled)
{
    _enabled = enabled;
}

CtrlType SocketCtrl::getType() const
{
    return CTRL_TYPE_SOCKET;
}

void SocketCtrl::setName(const String &name)
{
    _name = name;
}

const String &SocketCtrl::getName() const
{
    return _name;
}
