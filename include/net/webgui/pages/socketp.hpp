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

#ifndef __SOCKET_PAGE_HPP__
#define __SOCKET_PAGE_HPP__

#include <SettingsAsync.h>
#include "pages.hpp"
#include "controllers/socket.hpp"

class SocketPage
{
public:
    WebGuiPage build(sets::Builder& b);
    void update(sets::Updater& upd);

private:
    size_t _getCurRelay(const Socket *socket, const std::vector<GpioPin *> &relays) const;
    size_t _getCurButton(const Socket *socket, const std::vector<GpioPin *> &buttons) const;
    size_t _getCurLed(const Socket *socket, const std::vector<GpioPin *> &led) const;
};

#endif /* __SOCKET_PAGE_HPP__ */

