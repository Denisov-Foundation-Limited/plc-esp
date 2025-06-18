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

#include "net/pages/socketp.hpp"
#include "net/pages/elements.hpp"

/*********************************************************************/
/*                                                                   */
/*                          PUBLIC FUNCTIONS                         */
/*                                                                   */
/*********************************************************************/

WebGuiPage SocketPageClass::build(sets::Builder& b)
{
    std::vector<Socket *>   sockets;
    std::vector<GpioPin *>  relays, buttons, leds;
    String                  sSensors = "", sBtns = "", sRlys = "", sLeds = "";
    WebGuiPage              curPage = WEB_PAGE_SOCKETS;

    SocketCtrl.getSockets(false, sockets);
    
    if (b.beginGroup(F("Розетки"))) {
        if (b.Switch(WEB_GUI_CTRL_SOCKET_ENABLE, F("Включен"), &SocketCtrl.getEnabled())) {
            b.reload();
        }
        if (b.beginButtons()) {
            if (b.Button(F("Включить все"))) {
                std::vector<Socket *> socks;
                SocketCtrl.getSockets(true, socks);
                for (auto s : socks) {
                    SocketCtrl.setStatus(s, true, true);
                }
                b.reload();
            }
            if (b.Button(F("Отключить все"), sets::Colors::Red)) {
                std::vector<Socket *> socks;
                SocketCtrl.getSockets(true, socks);
                for (auto s : socks) {
                    SocketCtrl.setStatus(s, false, true);
                }
                b.reload();
            }
            b.endButtons();
        }
        if (b.Button(F("Назад"), sets::Colors::Aqua)) {
            curPage = WEB_PAGE_CONTROLLERS;
            b.reload();
        }
        b.endGroup();
    }

    if (SocketCtrl.getEnabled()) {
        Gpio.getPinsByType(GPIO_TYPE_RELAY, relays);
        Gpio.getPinsByType(GPIO_TYPE_INPUT, buttons);
        Gpio.getPinsByType(GPIO_TYPE_LED, leds);
        for (auto btn : buttons) {
            sBtns += btn->name + ";";
        }
        for (auto rly : relays) {
            sRlys += rly->name + ";";
        }
        for (auto led : leds) {
            sLeds += led->name + ";";
        }

        for (auto *socket : sockets) {
            if (b.beginGroup(String(F("Розетка #")) + String(socket->id))) {
                if (b.Switch(su::SH(String("ctrl_socket_en" + String(socket->id)).c_str()), F("Включен"), &socket->enabled)) {
                    b.reload();
                }
                if (socket->enabled) {
                    size_t  curRelay = _getCurRelay(socket, relays);
                    size_t  curButton = _getCurButton(socket, buttons);
                    size_t  curLed = _getCurLed(socket, leds);

                    b.Input(su::SH(String("ctrl_socket_name" + String(socket->id)).c_str()), F("Имя"), &socket->name);
                    if (b.Switch(su::SH(String("ctrl_socket_sts_" + String(socket->id)).c_str()), F("Статус"), &SocketCtrl.getStatus(socket))) {
                        SocketCtrl.setStatus(socket, b.build.value.toBool(), true);
                    }
                    if (b.Select(su::SH(("ctrl_socket_rly" + String(socket->id)).c_str()), F("Реле"), sRlys, &curRelay)) {
                        if (b.build.value.toInt32() < relays.size()) {
                            socket->relay = relays[b.build.value.toInt32()];
                            SocketCtrl.begin(false);
                        } else {
                            Log.error(F("SOCKETP"), F("Incorrect GPIO relay id"));
                        }
                    }
                    if (b.Select(su::SH(("ctrl_socket_btn" + String(socket->id)).c_str()), F("Кнопка"), sBtns, &curButton)) {
                        if (b.build.value.toInt32() < buttons.size()) {
                            socket->button = buttons[b.build.value.toInt32()];
                            SocketCtrl.begin(false);
                        } else {
                            Log.error(F("SOCKETP"), F("Incorrect GPIO button id"));
                        }
                    }
                    if (b.Select(su::SH(("ctrl_socket_led" + String(socket->id)).c_str()), F("Индикатор"), sLeds, &curLed)) {
                        if (b.build.value.toInt32() < leds.size()) {
                            socket->led = leds[b.build.value.toInt32()];
                            SocketCtrl.begin(false);
                        } else {
                            Log.error(F("SOCKETP"), F("Incorrect GPIO button id"));
                        }
                    }
                }
                b.endGroup();
            }
        }
    }

    return curPage;
}

void SocketPageClass::update(sets::Updater& upd)
{
    std::vector<Socket *>    sockets;

    SocketCtrl.getSockets(false, sockets);

    for (auto *socket : sockets) {
        upd.update(su::SH(String("ctrl_socket_en" + String(socket->id)).c_str()), socket->enabled);
        if (socket->enabled) {
            upd.update(su::SH(String("ctrl_socket_name" + String(socket->id)).c_str()), socket->name);
        }
    }    
}

/*********************************************************************/
/*                                                                   */
/*                          PRIVATE FUNCTIONS                        */
/*                                                                   */
/*********************************************************************/

size_t SocketPageClass::_getCurRelay(const Socket *socket, const std::vector<GpioPin *> &relays) const
{
    size_t  curRelay = relays.size();

    if (socket->relay != nullptr) {
        for (size_t i = 0; i < relays.size(); i++) {
            if (relays[i]->id == socket->relay->id) {
                curRelay = i;
                break;
            }
        }
    }

    return curRelay;
}

size_t SocketPageClass::_getCurButton(const Socket *socket, const std::vector<GpioPin *> &buttons) const
{
    size_t  curButton = buttons.size();

    if (socket->button != nullptr) {
        for (size_t i = 0; i < buttons.size(); i++) {
            if (buttons[i]->id == socket->button->id) {
                curButton = i;
                break;
            }
        }
    }

    return curButton;
}

size_t SocketPageClass::_getCurLed(const Socket *socket, const std::vector<GpioPin *> &leds) const
{
    size_t  curLED = leds.size();

    if (socket->led != nullptr) {
        for (size_t i = 0; i < leds.size(); i++) {
            if (leds[i]->id == socket->led->id) {
                curLED = i;
                break;
            }
        }
    }

    return curLED;
}

SocketPageClass SocketPage;
