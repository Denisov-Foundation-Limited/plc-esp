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

#ifndef __SOCKET_HPP__
#define __SOCKET_HPP__

#include <Arduino.h>
#include <vector>

#include "core/ifaces/gpio.hpp"

#define SOCKET_BUTTON_WAIT_MS   1000
#define SOCKET_BUTTON_READ_MS   100
#define SOCKET_COUNT            32

typedef struct {
    size_t      id;
    String      name;
    bool        status;
    bool        reading;
    unsigned    timer;
    GpioPin     *button;
    GpioPin     *relay;
    GpioPin     *led;
    bool        enabled;
} Socket;

class SocketCtrlClass
{
public:
    SocketCtrlClass();
    void setEnabled(bool enabled);
    bool getEnabled() const;
    bool setSocket(size_t id, Socket *sock);
    void getSockets(bool enabled, std::vector<Socket *> &sockets);
    bool getSocket(const String &name, Socket **sock);
    bool getSocket(size_t index, Socket **sock);
    void setStatus(Socket *sock, bool status, bool save);
    bool getStatus(Socket *sock) const;
    void begin(bool load);
    void loop();

private:
    std::array<Socket, SOCKET_COUNT>    _sockets;
    bool                                _reading = false;
    unsigned                            _timer;
    unsigned                            _curSocket = 0;
    bool                                _enabled = false;
    String                              _name;

    void _readButton(Socket *sock);
    bool _loadStates();
};

extern SocketCtrlClass SocketCtrl;

#endif /* __SOCKET_HPP__ */
