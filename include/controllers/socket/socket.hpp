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

#define SOCKET_BUTTON_WAIT_MS   1000
#define SOCKET_BUTTON_READ_MS   100

typedef enum {
    SOCK_IF_RELAY,
    SOCK_IF_BUTTON,
    SOCK_IF_MAX
} SockIfType;

class SocketCtrl;

class Socket
{
public:
    Socket(const String &name);
    void setStatus(bool status, bool save=false);
    bool getStatus() const;
    void setName(const String &name);
    const String &getName() const;
    void begin();
    void loop();
    void readButton();
    void setInterface(SockIfType type, Interface *iface);
    Interface *getInterface(SockIfType type) const;
    void setController(SocketCtrl *ctrl);

private:
    String      _name;
    bool        _status = false;
    bool        _reading = false;
    unsigned    _timer;
    IfGPIO      *_gpio[SOCK_IF_MAX] = { nullptr, nullptr };
    SocketCtrl  *_ctrl = nullptr;
};

class SocketCtrl : public Controller
{
public:
    SocketCtrl(const String &name);
    void addSocket(Socket *sock);
    const std::vector<Socket *> &getSockets();
    bool isExists(const String &name);
    Socket *getSocket(const String &name);
    void begin();
    void loop();
    bool getEnabled() const;
    void setEnabled(bool enabled);
    CtrlType getType() const;
    void setName(const String &name);
    const String &getName() const;
    void remove(size_t idx);

private:
    std::vector<Socket *>   _sockets;
    bool                    _reading = false;
    unsigned                _timer;
    unsigned                _curSocket = 0;
    bool                    _enabled;
    String                  _name;
};

#endif /* __SOCKET_HPP__ */
