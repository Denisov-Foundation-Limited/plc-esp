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

#include "db/socketdb.hpp"

void SocketDB::setStatus(const String &socket, bool status)
{
    auto data = Database::getData();
    (*data)[socket] = status;
}

bool SocketDB::getStatus(const String &socket, bool &status)
{
    auto data = Database::getData();
    if ((*data)[socket].isNull()) {
        return false;
    } else {
        status = (*data)[socket];
    }
    return true;
}
