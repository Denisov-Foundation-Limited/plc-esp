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

#ifndef __SOCKET_DB_HPP__
#define __SOCKET_DB_HPP__

#include <Arduino.h>

#include "database.hpp"

class SocketDB : public Database
{
public:
    void setStatus(const String &socket, bool status);
    bool getStatus(const String &socket, bool &status);
};

#endif /* __SOCKET_DB_HPP__ */
