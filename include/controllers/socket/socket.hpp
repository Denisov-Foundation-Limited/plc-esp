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

#ifndef __SOCKET_HPP__
#define __SOCKET_HPP__

#include <Arduino.h>
#include <vector>

#include "core/ifaces/gpio.hpp"
#include "controllers/ctrl.hpp"

#define SOCKET_BUTTON_WAIT_MS   600
#define SOCKET_BUTTON_READ_MS   100

typedef enum {
    SOCK_IF_RELAY,
    SOCK_IF_BUTTON,
    SOCK_IF_MAX
} SockIfType;

class Socket
{
public:
    Socket(const String &name)
    {
        _name = name;
    }

    void setStatus(bool status, bool save=false)
    {
        _status = status;
    }

    bool getStatus() const
    {
        return _status;
    }

    void setName(const String &name)
    {
        _name = name;
    }

    const String &getName() const
    {
        return _name;
    }

    void begin()
    {
        if (_gpio[SOCK_IF_RELAY] != nullptr) {
            _gpio[SOCK_IF_RELAY]->setMode(GPIO_MOD_OUTPUT);
            _gpio[SOCK_IF_RELAY]->write(false);
        }
        if (_gpio[SOCK_IF_BUTTON] != nullptr) {
            _gpio[SOCK_IF_BUTTON]->setMode(GPIO_MOD_INPUT);
        }
    }

    void loop()
    {
        if (_reading) {
            if ((millis() - _timer) >= SOCKET_BUTTON_WAIT_MS) {
                _reading = false;
            }
        }
    }

    void readButton()
    {
        if (_gpio[SOCK_IF_BUTTON] != nullptr) {
            if (_gpio[SOCK_IF_BUTTON]->read() && !_reading) {
                setStatus(!getStatus(), true);
                _reading = true;
                _timer = millis();
            }
        }
    }

    void setGpio(SockIfType type, Interface *iface)
    {
        _gpio[type] = static_cast<GPIOIface *>(iface);
    }

    Interface *getGpio(SockIfType type) const
    {
        return _gpio[type];
    }

private:
    String      _name;
    bool        _status = false;
    bool        _reading = false;
    unsigned    _timer;
    GPIOIface   *_gpio[SOCK_IF_MAX];
};

class SocketCtrl : public Controller
{
public:
    SocketCtrl(const String &name)
    {
        _name = name;
    }

    void addSocket(Socket *sock)
    {
        _sockets.push_back(sock);
    }

    const std::vector<Socket *> &getSockets()
    {
        return _sockets;
    }

    bool isExists(const String &name)
    {
        for (auto sock : _sockets) {
            if (sock->getName() == name) {
                return true;
            }
        }
        return false;
    }

    Socket *getSocket(const String &name)
    {
        for (auto sock : _sockets) {
            if (sock->getName() == name) {
                return sock;
            }
        }
        return nullptr;
    }

    void begin()
    {
        if (!_enabled) return;
        for (auto s : _sockets) {
            s->begin();
        }
    }

    void loop()
    {
        if (!_enabled) return;

        for (auto s : _sockets) {
            s->loop();
        }

        if (_reading) {
            _sockets[_curSocket]->readButton();
            if (_curSocket == _sockets.size() - 1) {
                _reading = false;
                _timer = millis();
                _curSocket = 0;
            }
            _curSocket++;
        } else {
            if ((millis() - _timer) >= SOCKET_BUTTON_READ_MS) {
                _reading = true;
            }
        }
    }

    bool getEnabled() const
    {
        return _enabled;
    }

    void setEnabled(bool enabled)
    {
        _enabled = enabled;
    }

    CtrlType getType() const
    {
        return CTRL_TYPE_SOCKET;
    }

    void setName(const String &name)
    {
        _name = name;
    }

    const String &getName() const
    {
        return _name;
    }

private:
    std::vector<Socket *>   _sockets;
    bool                    _reading = false;
    unsigned                _timer;
    unsigned                _curSocket;
    bool                    _enabled;
    String                  _name;
};

#endif /* __SOCKET_HPP__ */
