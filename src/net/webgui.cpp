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

#include "net/webgui.hpp"
#include "utils/configs.hpp"
#include "net/core/wifi.hpp"
#include "controllers/meteo.hpp"
#include "controllers/socket.hpp"
#include "controllers/climate.hpp"
#include "controllers/security.hpp"
#include "controllers/tank.hpp"
#include "core/clock.hpp"

#include <StringUtils.h>

/*********************************************************************/
/*                                                                   */
/*                          PUBLIC FUNCTIONS                         */
/*                                                                   */
/*********************************************************************/

void WebGUIClass::begin()
{
    onBuild([this](sets::Builder& b) {
        _buildMenu(b);

        switch (_curPage) {
            case WEB_PAGE_MAIN:
                _buildMainPage(b);
                break;
            case WEB_PAGE_TELEGRAM:
                _buildTgBotPage(b);
                break;
            case WEB_PAGE_CONTROLLERS:
                _buildCtrlsPage(b);
                break;
            case WEB_PAGE_SETTINGS:
                _buildSettingsPage(b);
                break;
            case WEB_PAGE_SOCKETS:
                _buildSocketsPage(b);
                break;
            case WEB_PAGE_METEO:
                _buildMeteoPage(b);
                break;
            case WEB_PAGE_CLIMATE:
                _buildClimatePage(b);
                break;
            case WEB_PAGE_SECURITY:
                _buildSecurityPage(b);
                break;
            case WEB_PAGE_TANK:
                _buildTankPage(b);
                break;
        }
    });

    onUpdate([this](sets::Updater& upd) {
        switch (_curPage) {
            case WEB_PAGE_MAIN:
                _updateMainPage(upd);
                break;
            case WEB_PAGE_TELEGRAM:
                _updateTgBotPage(upd);
                break;
            case WEB_PAGE_CONTROLLERS:
                _updateCtrlsPage(upd);
                break;
            case WEB_PAGE_SOCKETS:
                _updateSocketsPage(upd);
                break;
            case WEB_PAGE_SETTINGS:
                _updateSettingsPage(upd);
                break;
            case WEB_PAGE_METEO:
                _updateMeteoPage(upd);
                break;
            case WEB_PAGE_CLIMATE:
                _updateClimatePage(upd);
                break;
            case WEB_PAGE_SECURITY:
                _updateSecurityPage(upd);
                break;
            case WEB_PAGE_TANK:
                _updateTankPage(upd);
                break;
        }
    });

    setTitle(F("Future City PLC"));
    setPass(_password);
    setUpdatePeriod(100);
    SettingsAsync::begin();
}

void WebGUIClass::loop()
{
    tick();
}

void WebGUIClass::setPassword(const String &pwd)
{
    _password = pwd;
}

const String &WebGUIClass::getPassword()
{
    return _password;
}

/*********************************************************************/
/*                                                                   */
/*                          PRIVATE FUNCTIONS                        */
/*                                                                   */
/*********************************************************************/

void WebGUIClass::_buildMenu(sets::Builder& b)
{
    if (b.beginGroup(F("Меню"))) {
        b.beginButtons();
        
        if (b.Button(WEB_GUI_MENU_BTN_NET, F("Сеть"), sets::Colors::Aqua)) {
            _curPage = WEB_PAGE_MAIN;
            b.reload();
        }
        if (b.Button(WEB_GUI_MENU_BTN_TG, F("Telegram"), sets::Colors::Aqua)) {
            _curPage = WEB_PAGE_TELEGRAM;
            b.reload();
        }
        b.endButtons();
        b.beginButtons();
        if (b.Button(WEB_GUI_MENU_BTN_CTRL, F("Контроллеры"), sets::Colors::Aqua)) {
            _curPage = WEB_PAGE_CONTROLLERS;
            b.reload();
        }
        if (b.Button(WEB_GUI_MENU_BTN_CFG, F("Система"), sets::Colors::Aqua)) {
            _curPage = WEB_PAGE_SETTINGS;
            b.reload();
        }
        b.endButtons();
        b.endGroup();
    }
}

void WebGUIClass::_buildMainPage(sets::Builder& b)
{
    if (b.beginGroup(F("Беспроводная сеть"))) {
        if (b.Switch(WEB_GUI_MAIN_WIFI_EN, F("Enabled"), &Wireless.getEnabled())) {
            Wireless.setEnabled(b.build.value.toBool());
            b.reload();
        }
        if (Wireless.getEnabled()) {
            if (b.Input(WEB_GUI_MAIN_WIFI_SSID, F("SSID"), &Wireless.getSSID())) {
                Wireless.setCreds(b.build.value, Wireless.getPasswd());
            }
            if (b.Pass(WEB_GUI_MAIN_WIFI_PWD, F("Password"), &Wireless.getPasswd())) {
                Wireless.setCreds(Wireless.getSSID(), b.build.value);
            }
            if (b.Switch(WEB_GUI_MAIN_WIFI_AP, F("AP"), &Wireless.getAP())) {
                Wireless.setAP(b.build.value.toBool());
            }
            b.Label(WEB_GUI_MAIN_WIFI_IP, F("IP"), Wireless.getIP());
            b.LED(WEB_GUI_MAIN_WIFI_STATUS, F("Connection"), Wireless.getStatus() == WL_CONNECTED);
        }
        b.endGroup();
    }

    b.Label(F("Copyright"), F("Denisov Foundation Limited 2024-2025"));
}

void WebGUIClass::_updateMainPage(sets::Updater& upd)
{
    upd.update(WEB_GUI_MAIN_WIFI_STATUS, Wireless.getStatus() == WL_CONNECTED);
    upd.update(WEB_GUI_MAIN_WIFI_IP, Wireless.getIP());
    upd.update(WEB_GUI_MAIN_WIFI_AP, Wireless.getAP());
    upd.update(WEB_GUI_MAIN_WIFI_EN, Wireless.getEnabled());
}

void WebGUIClass::_buildTgBotPage(sets::Builder& b)
{
    if (b.beginGroup(F("Telegram"))) {
        if (b.Switch(WEB_GUI_TG_EN, F("Enabled"), &TgBot.getEnabled())) {
            TgBot.setEnabled(b.build.value.toBool());
            TgBot.begin();
            b.reload();
        }
        if (TgBot.getEnabled()) {
            String token = TgBot.getToken();
            if (b.Pass(WEB_GUI_TG_TOKEN, F("Токен"), &token)) {
                TgBot.setToken(b.build.value);
                TgBot.begin();
            }
            auto mode = TgBot.getPollMode();
            uint8_t modeNum = 0;
            switch (mode) {
                case fb::Poll::Long:
                    modeNum = 0;
                    break;
                case fb::Poll::Sync:
                    modeNum = 1;
                    break;
                case fb::Poll::Async:
                    modeNum = 2;
                    break;
            }
            if (b.Select(WEB_GUI_TG_POLL, F("Режим"), F("Long;Sync;Async"), &modeNum)) {
                switch (b.build.value.toInt32()) {
                    case 0:
                        TgBot.setPollMode(fb::Poll::Long, TgBot.getPollPeriod());
                        break;
                    case 1:
                        TgBot.setPollMode(fb::Poll::Sync, TgBot.getPollPeriod());
                        break;
                    case 2:
                        TgBot.setPollMode(fb::Poll::Async, TgBot.getPollPeriod());
                        break;
                }
                TgBot.begin();
            }

            uint16_t period = TgBot.getPollPeriod();
            if (b.Slider(WEB_GUI_TG_PERIOD, "Период", 100, 100000, 10, F("msec"), &period)) {
                TgBot.setPollMode(TgBot.getPollMode(), b.build.value.toInt32());
                TgBot.begin();
            }
            b.Label(WEB_GUI_TG_LAST_ID, F("LastID"), String(TgBot.getLastID()));
        }

        if (b.beginMenu(F("Настройка"))) {
            String                  sUsers = "";
            std::vector<TgUser *>   users;

            TgBot.getEnabledUsers(users);

            for (size_t i = 0; i < TG_USERS_COUNT; i++) {
                sUsers += ("#"+String(i + 1));
                if (i < (TG_USERS_COUNT - 1)) {
                    sUsers += ";";
                }
            }

            if (b.beginGroup(F("Редактировать"))) {
                if (b.Select(WEB_GUI_CTRL_SOCKET_SEL, F("Выбрать"), sUsers, (uint8_t *)&_tgUser.curUser)) {
                    TgUser *user;
                    TgBot.getUser(_tgUser.curUser, &user);
                    _tgUser.Name = user->name;
                    _tgUser.Enabled = user->enabled;
                    _tgUser.ChatID = user->chatId;
                    _tgUser.Admin = user->admin;
                    _tgUser.Notify = user->notify;
                }

                b.Input(WEB_GUI_TG_USER_NAME, F("Имя"), &_tgUser.Name);
                b.Switch(WEB_GUI_TG_USER_EN, F("Включен"), &_tgUser.Enabled);
                b.Number(WEB_GUI_TG_USER_CHATID, F("ChatID"), &_tgUser.ChatID);
                b.Switch(WEB_GUI_TG_USER_ADMIN, F("Админ"), &_tgUser.Admin);
                b.Switch(WEB_GUI_TG_USER_NTF, F("Уведомления"), &_tgUser.Notify);

                if (b.Button(F("Применить"))) {
                    TgUser *user;
                    if (TgBot.getUser(_tgUser.curUser, &user)) {
                        user->name = _tgUser.Name;
                        user->enabled = _tgUser.Enabled;
                        user->admin = _tgUser.Admin;
                        user->chatId = _tgUser.ChatID;
                        user->notify = _tgUser.Notify;
                        b.reload();
                    }
                }
            }
            b.endGroup();

            if (b.beginGroup(F("Активные пользователи"))) {
                size_t i = 1;
                for (auto user : *TgBot.getUsers()) {
                    b.LED(su::SH(String("tg_users_en_" + String(i)).c_str()), 
                        "#"+String(i), user.enabled);
                    i++;
                }
                b.endGroup();
            }
            b.endMenu();
        }
        b.endGroup();
    }

    std::vector<TgUser *> users;
    TgBot.getEnabledUsers(users);
    size_t i = 1;
    for (auto *user : users) {
        if (b.beginGroup("Пользователь #" +String(i))) {
            b.Label(su::SH(("tg_user_name_"+String(i)).c_str()), "Имя", user->name);
            b.Label(su::SH(("tg_user_chid_"+String(i)).c_str()), "ChatID", String(user->chatId));
            b.LED(su::SH(("tg_user_admin_"+String(i)).c_str()), "Админ", user->admin);
            b.LED(su::SH(("tg_user_ntf_"+String(i)).c_str()), "Уведомления", user->notify);
            b.endGroup();
        }
        i++;
    }
}

void WebGUIClass::_updateTgBotPage(sets::Updater& upd)
{
    upd.update(WEB_GUI_TG_EN, TgBot.getEnabled());
    upd.update(WEB_GUI_TG_TOKEN, TgBot.getToken());
    upd.update(WEB_GUI_TG_PERIOD, TgBot.getPollPeriod());

    uint8_t modeNum = 0;
    switch (TgBot.getPollMode()) {
        case fb::Poll::Long:
            modeNum = 0;
            break;
        case fb::Poll::Sync:
            modeNum = 1;
            break;
        case fb::Poll::Async:
            modeNum = 2;
            break;
    }
    upd.update(WEB_GUI_TG_POLL, modeNum);

    std::vector<TgUser *> users;
    TgBot.getEnabledUsers(users);
    size_t i = 1;
    for (auto *user : users) {
        upd.update(su::SH(("tg_user_name_"+String(i)).c_str()), user->name);
        upd.update(su::SH(("tg_user_chid_"+String(i)).c_str()), user->chatId);
        upd.update(su::SH(("tg_user_admin_"+String(i)).c_str()), user->admin);
        upd.update(su::SH(("tg_user_ntf_"+String(i)).c_str()), user->notify);
        i++;
    }

    upd.update(WEB_GUI_TG_USER_NAME, _tgUser.Name);
    upd.update(WEB_GUI_TG_USER_EN, _tgUser.Enabled);
    upd.update(WEB_GUI_TG_USER_CHATID, _tgUser.ChatID);
    upd.update(WEB_GUI_TG_USER_ADMIN, _tgUser.Admin);
    upd.update(WEB_GUI_TG_USER_NTF, _tgUser.Notify);

    i = 1;
    for (auto user : *TgBot.getUsers()) {
        upd.update(su::SH(String("tg_users_en_" + String(i)).c_str()), user.enabled);
        i++;
    }
}

void WebGUIClass::_buildCtrlsPage(sets::Builder& b)
{
    if (b.beginGroup(F("Контроллеры"))) {
        if (b.beginButtons()) {
            if (b.Button(WEB_GUI_CTRL_SOCKET, F("Розетки"))) {
                _curPage = WEB_PAGE_SOCKETS;
                b.reload();
            }
            b.endButtons();
        }
        if (b.beginButtons()) {
            if (b.Button(WEB_GUI_CTRL_METEO, F("Метео"))) {
                _curPage = WEB_PAGE_METEO;
                b.reload();
            }
            b.endButtons();
        }
        if (b.beginButtons()) {
            if (b.Button(WEB_GUI_CTRL_CLIMATE, F("Климат"))) {
                _curPage = WEB_PAGE_CLIMATE;
                b.reload();
            }
            b.endButtons();
        }
        if (b.beginButtons()) {
            if (b.Button(WEB_GUI_CTRL_SECURITY, F("Охрана"))) {
                _curPage = WEB_PAGE_SECURITY;
                b.reload();
            }
            b.endButtons();
        }
        b.endGroup();
        if (b.beginButtons()) {
            if (b.Button(WEB_GUI_CTRL_TANK, F("Баки"))) {
                _curPage = WEB_PAGE_TANK;
                b.reload();
            }
            b.endButtons();
        }
    }

    if (b.Button(WEB_GUI_CTRL_BACK, F("Назад"), sets::Colors::Aqua)) {
        _curPage = WEB_PAGE_MAIN;
        b.reload();
    }
}

void WebGUIClass::_updateCtrlsPage(sets::Updater& upd)
{
}

void WebGUIClass::_buildSocketsPage(sets::Builder& b)
{
    if (b.beginGroup(F("Общее"))) {
        if (b.Switch(WEB_GUI_CTRL_SOCKET_ENABLE, F("Включен"), &SocketCtrl.getEnabled())) {
            b.reload();
        }
        if (SocketCtrl.getEnabled()) {
            if (b.beginMenu(F("Настройки"))) {
                if (b.beginGroup(F("Редактировать"))) {
                    String                  sSocks = "", sBtns = F("N/U;"), sRlys = F("N/U;"), sLeds = F("N/U;");
                    std::vector<GpioPin *>  pins;
                    size_t                  bt = 1, rl = 1, ld = 1;

                    Gpio.getPins(pins);

                    for (size_t i = 0; i < SOCKET_COUNT; i++) {
                        sSocks += ("#"+String(i + 1));
                        if (i < (SOCKET_COUNT - 1)) {
                            sSocks += ";";
                        }
                    }
                    for (auto pin : pins) {
                        if (pin->type == GPIO_TYPE_INPUT) {
                            if (pin->ext == nullptr) {
                                sBtns += "in-1/0/" + String(bt) + ";";
                            } else {
                                sBtns += "in-" + String(pin->ext->i2c->id) + "/" + String(pin->ext->id) + "/" + String(bt) + ";";
                            }
                            bt++;
                        }
                        if (pin->type == GPIO_TYPE_RELAY) {
                            if (pin->ext == nullptr) {
                                sRlys += "rly-1/0/" + String(rl) + ";";
                            } else {
                                sRlys += "rly-" + String(pin->ext->i2c->id) + "/" + String(pin->ext->id) + "/" + String(rl) + ";";
                            }
                            rl++;
                        }
                        if (pin->type == GPIO_TYPE_LED) {
                            if (pin->ext == nullptr) {
                                sLeds += "led-1/0/" + String(ld) + ";";
                            } else {
                                sLeds += "led-" + String(pin->ext->i2c->id) + "/" + String(pin->ext->id) + "/" + String(ld) + ";";
                            }
                            ld++;
                        }
                    }
                    sBtns.remove(sBtns.length() - 1);
                    sRlys.remove(sRlys.length() - 1);
                    sLeds.remove(sLeds.length() - 1);

                    if (b.Select(WEB_GUI_CTRL_SOCKET_SEL, F("Выбрать"), sSocks, (uint8_t *)&_socket.curSock)) {
                        Socket *sock;
                        SocketCtrl.getSocket(_socket.curSock, &sock);
                        _socket.Name = sock->name;
                        _socket.Enabled = sock->enabled;
                        bt = 1; rl = 1; ld = 1;
                        for (auto pin : pins) {
                            if (pin->type == GPIO_TYPE_INPUT) {
                                if (sock->button != nullptr && pin->id == sock->button->id) {
                                    _socket.curBtn = bt;
                                    break;
                                } else {
                                    _socket.curBtn = 0;
                                }
                                bt++;
                            }
                        }
                        for (auto pin : pins) {
                            if (pin->type == GPIO_TYPE_RELAY) {
                                if (sock->relay != nullptr && pin->id == sock->relay->id) {
                                    _socket.curRly = rl;
                                    break;
                                } else {
                                    _socket.curRly = 0;
                                }
                                rl++;
                            }
                        }
                        for (auto pin : pins) {
                            if (pin->type == GPIO_TYPE_LED) {
                                if (sock->led != nullptr && pin->id == sock->led->id) {
                                    _socket.curLED = ld;
                                    break;
                                } else {
                                    _socket.curLED = 0;
                                }
                                ld++;
                            }
                        }
                    }

                    b.Input(WEB_GUI_CTRL_SOCKET_NAME, F("Имя"), &_socket.Name);
                    b.Switch(WEB_GUI_CTRL_SOCKET_ENABLE_SOCK, F("Включен"), &_socket.Enabled);
                    b.Select(WEB_GUI_CTRL_SOCKET_RLY, F("Реле"), sRlys, (uint8_t *)&_socket.curRly);
                    b.Select(WEB_GUI_CTRL_SOCKET_BTN, F("Кнопка"), sBtns, (uint8_t *)&_socket.curBtn);
                    b.Select(WEB_GUI_CTRL_SOCKET_LED, F("Индикатор"), sLeds, (uint8_t *)&_socket.curLED);

                    if (b.Button(F("Применить"))) {
                        Socket *sock;
                        if (SocketCtrl.getSocket(_socket.curSock, &sock)) {
                            sock->name = _socket.Name;
                            sock->enabled = _socket.Enabled;
                            bt = 1; rl = 1; ld = 1;
                            for (auto pin : pins) {
                                if (pin->type == GPIO_TYPE_INPUT) {
                                    if (_socket.curBtn == bt) {
                                        Gpio.getPinById(pin->id, &sock->button);
                                    }
                                    bt++;
                                }
                                if (pin->type == GPIO_TYPE_RELAY) {
                                    if (_socket.curRly == rl) {
                                        Gpio.getPinById(pin->id, &sock->relay);
                                    }
                                    rl++;
                                }
                                if (pin->type == GPIO_TYPE_LED) {
                                    if (_socket.curLED == ld) {
                                        Gpio.getPinById(pin->id, &sock->led);
                                    }
                                    ld++;
                                }
                            }
                        }
                        SocketCtrl.begin();
                        b.reload();
                    }
                    b.endGroup();
                }

                if (b.beginGroup(F("Активные розетки"))) {
                    size_t i = 1;
                    for (auto sock : *SocketCtrl.getSockets()) {
                        b.LED(su::SH(String("ctrl_sock_en_" + String(i)).c_str()), 
                            "#"+String(i), sock.enabled);
                        i++;
                    }
                    b.endGroup();
                }
                b.endMenu();
            }
            b.beginButtons();           
            if (b.Button(WEB_GUI_CTRL_SOCKET_ON_ALL, F("Включить все"))) {
                std::vector<Socket *> socks;
                SocketCtrl.getEnabledSockets(socks);
                for (auto s : socks) {
                    SocketCtrl.setStatus(s, true, true);
                }
                b.reload();
            }
            if (b.Button(WEB_GUI_CTRL_SOCKET_OFF_ALL, F("Отключить все"), sets::Colors::Red)) {
                std::vector<Socket *> socks;
                SocketCtrl.getEnabledSockets(socks);
                for (auto s : socks) {
                    SocketCtrl.setStatus(s, false, true);
                }
                b.reload();
            }
            b.endButtons();
        }
        b.endGroup();
    }

    if (SocketCtrl.getEnabled()) {
        if (b.beginGroup(F("Розетки"))) {
            std::vector<Socket *> socks;
            SocketCtrl.getEnabledSockets(socks);
            for (size_t i = 0; i < socks.size(); i++) {
                if (b.Switch(su::SH(String("ctrl_sock_sw_" + String(i)).c_str()), socks[i]->name, &SocketCtrl.getStatus(socks[i]))) {
                    SocketCtrl.setStatus(socks[i], b.build.value.toBool(), true);
                }
            }
            b.endGroup();
        }
    }

    if (b.Button(WEB_GUI_CTRL_SOCKET_BACK, F("Назад"), sets::Colors::Aqua)) {
        _curPage = WEB_PAGE_CONTROLLERS;
        b.reload();
    }
}

void WebGUIClass::_updateSocketsPage(sets::Updater& upd)
{
    upd.update(WEB_GUI_CTRL_SOCKET_NAME, _socket.Name);
    upd.update(WEB_GUI_CTRL_SOCKET_ENABLE_SOCK, _socket.Enabled);
    upd.update(WEB_GUI_CTRL_SOCKET_RLY, _socket.curRly);
    upd.update(WEB_GUI_CTRL_SOCKET_BTN, _socket.curBtn);
    upd.update(WEB_GUI_CTRL_SOCKET_LED, _socket.curLED);

    size_t i = 1;
    for (auto sock : *SocketCtrl.getSockets()) {
        upd.update(su::SH(String("ctrl_sock_en_" + String(i)).c_str()), sock.enabled);
        i++;
    }

    std::vector<Socket *> socks;
    SocketCtrl.getEnabledSockets(socks);
    for (size_t i = 0; i < socks.size(); i++) {
        upd.update(su::SH(String("ctrl_sock_sw_" + String(i)).c_str()), SocketCtrl.getStatus(socks[i]));
    }
}

void WebGUIClass::_buildSettingsPage(sets::Builder& b)
{
    if (b.beginGroup(F("Часы"))) {
        Datime  time;
        Clock.getTime(time);

        b.Label(WEB_GUI_SYS_DATE, F("Дата"), time.dateToString());
        b.Label(WEB_GUI_SYS_TIME, F("Время"), time.timeToString());
        b.Label(WEB_GUI_SYS_UTC, F("UTC"), "+" + String(Clock.getUTC()));

        if (b.beginMenu(F("Настройка"))) {
            if (b.beginGroup(F("Часы"))) {
                uint32_t t = time.getUnix();
                if (b.DateTime(WEB_GUI_SYS_DTIME, F("Дата и время"), &t)) {
                    _time = b.build.value.toInt32();
                }
                b.Slider(WEB_GUI_SYS_UTC_SET, "UTC", CLOCK_UTC_MIN, CLOCK_UTC_MAX, 1, "h", &Clock.getUTC());
                if (b.Button(WEB_GUI_SYS_UTC_APPLY, F("Применить"))) {
                    Datime  t(_time);
                    Clock.setTimeUTC(t);
                    b.reload();
                }
                b.endGroup();
            }
            b.endMenu();
        }
        
        b.endGroup();
    }

    if (b.beginGroup(F("Охлаждение"))) {
        b.Number(WEB_GUI_SYS_TEMP, F("Температура"), &Plc.getBoardTemp());
        if (b.Switch(WEB_GUI_SYS_FAN_EN, F("Мониторинг"), &Plc.getFanEnabled())) {
            Plc.setFanEnabled(b.build.value.toBool());
        }
        b.LED(WEB_GUI_SYS_FAN_STATUS, F("Вентилятор"), &Plc.getFanStatus());
        b.endGroup();
    }

    if (b.beginGroup(F("Настройки"))) {
        b.beginButtons();
        if (b.Button(WEB_GUI_SYS_SAVE, F("Сохранить"))) {
            Configs.writeAll();
            _curPage = WEB_PAGE_MAIN;
            b.reload();
        }
        if (b.Button(WEB_GUI_SYS_DEL, F("Удалить"), sets::Colors::Red)) {
            Configs.eraseAll();
            _curPage = WEB_PAGE_MAIN;
            b.reload();
        }
        b.endButtons();
        b.endGroup();
    }

    if (b.beginGroup(F("Система"))) {
        b.beginButtons();
        if (b.Button(WEB_GUI_SYS_RESTART, F("Рестарт"))) {
            ESP.restart();
        }
        b.endButtons();
        b.endGroup();
    }
}

void WebGUIClass::_updateSettingsPage(sets::Updater& upd)
{
    Datime  time;
    Clock.getTime(time);

    upd.update(WEB_GUI_SYS_TEMP, Plc.getBoardTemp());
    upd.update(WEB_GUI_SYS_FAN_EN, Plc.getFanEnabled());
    upd.update(WEB_GUI_SYS_FAN_STATUS, Plc.getFanStatus());
    upd.update(WEB_GUI_SYS_DATE, time.dateToString());
    upd.update(WEB_GUI_SYS_TIME, time.timeToString());
    upd.update(WEB_GUI_SYS_UTC, String("+" + String(Clock.getUTC())));
}

void WebGUIClass::_buildMeteoPage(sets::Builder& b)
{
    std::vector<MeteoSensor *>    sensors;

    MeteoCtrl.getEnabledSensors(sensors);
    
    if (b.beginGroup(F("Общее"))) {
        if (b.Switch(WEB_GUI_CTRL_METEO_ENABLE, F("Включен"), &MeteoCtrl.getEnabled())) {
            b.reload();
        }

        if (MeteoCtrl.getEnabled()) {
            if (b.beginMenu(F("Настройка"))) {
                if (b.beginGroup(F("Редактирование"))) {
                    String sMeteo = "";

                    for (size_t i = 0; i < METEO_SENSOR_COUNT; i++) {
                        sMeteo += ("#"+String(i + 1));
                        if (i < (METEO_SENSOR_COUNT - 1)) {
                            sMeteo += ";";
                        }
                    }

                    if (b.Select(WEB_GUI_CTRL_METEO_SEL, F("Выбрать"), sMeteo, (uint8_t *)&_meteo.curSensor)) {
                        MeteoSensor *sens;
                        MeteoCtrl.getSensor(_meteo.curSensor, &sens);
                        _meteo.Name = sens->name;
                        _meteo.Enabled = sens->enabled;
                        _meteo.Addr = String(sens->addr, 16);
                        _meteo.curType = sens->type;
                    }

                    b.Input(WEB_GUI_CTRL_METEO_NAME, F("Имя"), &_meteo.Name);
                    b.Switch(WEB_GUI_CTRL_METEO_ENABLE_SENS, F("Включен"), &_meteo.Enabled);
                    b.Select(WEB_GUI_CTRL_METEO_TYPE, F("Тип"), F("AM2302;DS18B20;BME280;DHT22"), (uint8_t *)&_meteo.curType);
                    b.Input(WEB_GUI_CTRL_METEO_ADDR, F("Адрес"), &_meteo.Addr);

                    if (b.Button(F("Применить"))) {
                        MeteoSensor *sens;
                        if (MeteoCtrl.getSensor(_meteo.curSensor, &sens)) {
                            sens->name = _meteo.Name;
                            sens->enabled = _meteo.Enabled;
                            sens->type = static_cast<MeteoSensorType>(_meteo.curType);
                            sens->addr = strtoull(_meteo.Addr.c_str(), NULL, 16);
                        }
                        b.reload();
                    }
                    b.endGroup();
                }

                if (b.beginGroup(F("OneWire"))) {
                    std::vector<uint64_t> owSens;
                    MeteoCtrl.findDsSensors(owSens);
                    size_t i = 0;

                    for (auto s : owSens) {
                        i++;
                        b.Label("#"+String(i), String(s, 16));
                    }
                    if (b.Button(F("Поиск"), sets::Colors::Aqua)) {
                        b.reload();
                    }
                    b.endGroup();
                }
                if (b.beginGroup(F("Активные датчики"))) {
                    size_t i = 0;
                    for (auto sensor : MeteoCtrl.getSensors()) {
                        i++;
                        b.LED(su::SH(("meteo_sens_en" + String(sensor.id)).c_str()), "#"+String(i), sensor.enabled);
                    }
                    b.endGroup();
                }
                b.endMenu();
            }
        }
        if (b.Button(F("Назад"), sets::Colors::Aqua)) {
            _curPage = WEB_PAGE_CONTROLLERS;
            b.reload();
        }
        b.endGroup();
    }

    if (b.beginGroup("Датчики")) {
        for (auto sensor : sensors) {
            b.Label(su::SH(("meteo_sens_" + String(sensor->id)).c_str()), sensor->name, String(sensor->data.temp) + "°");
        }
        b.endGroup();
    }
}

void WebGUIClass::_updateMeteoPage(sets::Updater& upd)
{
    std::vector<MeteoSensor *>    sensors;

    MeteoCtrl.getEnabledSensors(sensors);

    upd.update(WEB_GUI_CTRL_METEO_NAME, _meteo.Name);
    upd.update(WEB_GUI_CTRL_METEO_ENABLE_SENS, _meteo.Enabled);
    upd.update(WEB_GUI_CTRL_METEO_TYPE, _meteo.curType);
    upd.update(WEB_GUI_CTRL_METEO_ADDR, _meteo.Addr);

    for (auto sensor : sensors) {
        String data = String(sensor->data.temp) + "°";
        upd.update(su::SH(("meteo_sens_" + String(sensor->id)).c_str()), data);
    }
}

void WebGUIClass::_buildClimatePage(sets::Builder& b)
{
    std::vector<ClimateZone *> zones;

    ClimateCtrl.getZones(false, zones);

    if (b.beginGroup(F("Общее"))) {
        if (b.Switch(WEB_GUI_CTRL_CLIMATE_ENABLE, F("Включен"), &ClimateCtrl.getEnabled())) {
            b.reload();
        }
        if (b.Button(F("Назад"), sets::Colors::Aqua)) {
            _curPage = WEB_PAGE_CONTROLLERS;
            b.reload();
        }
        b.endGroup();
    }

    if (ClimateCtrl.getEnabled()) {
        for (auto *zone : zones) {
            if (b.beginGroup(String(F("Зона #")) + String(zone->id))) {
                if (b.Switch(su::SH(String("ctrl_clmt_en_" + String(zone->id)).c_str()), F("Включить"), &zone->enabled)) {
                    b.reload();
                }
                if (zone->enabled) {
                    if (b.Switch(su::SH(String("ctrl_clmt_sts_" + String(zone->id)).c_str()), F("Статус"), &ClimateCtrl.getStatus(zone))) {
                        ClimateCtrl.setStatus(zone, b.build.value.toBool(), true);
                    }
                    b.Input(su::SH(String("ctrl_clmt_name_" + String(zone->id)).c_str()), F("Имя"), &zone->name);
                    if (b.Slider(su::SH(("ctrl_clmt_temp_" + String(zone->id)).c_str()), "Порог", -20, 40, 1, F("°"), &zone->temp)) {
                        ClimateCtrl.setTemp(zone, b.build.value.toInt(), true);
                    }
                    if (b.Slider(su::SH(("ctrl_clmt_dlt_" + String(zone->id)).c_str()), "Дельта", 0, 10, 1, F("°"), &zone->delta)) {
                        ClimateCtrl.setDelta(zone, b.build.value.toInt(), true);
                    }
                    unsigned modeNum = static_cast<unsigned>(zone->type);
                    if (b.Select(su::SH(("ctrl_clmt_mod_" + String(zone->id)).c_str()), F("Режим"), F("Обогрев;Охлаждение"), &modeNum)) {
                        switch (b.build.value.toInt32()) {
                            case 0:
                                zone->type = CLIMATE_TYPE_HEAT;
                                break;
                            case 1:
                                zone->type = CLIMATE_TYPE_COOL;
                                break;
                        }
                    }
                    b.Label(su::SH(("ctrl_clmt_sens_" + String(zone->id)).c_str()), F("Температура"), (zone->sensor != nullptr) ? (String(zone->sensor->data.temp) + "°") : String("N/A"));
                    b.LED(su::SH(("ctrl_clmt_work_" + String(zone->id)).c_str()), F("В работе"), &zone->work);
                }
                b.endGroup();
            }
        }
    }
}

void WebGUIClass::_updateClimatePage(sets::Updater& upd)
{
    std::vector<ClimateZone *> zones;

    ClimateCtrl.getZones(false, zones);

    for (auto *zone : zones) {
        upd.update(su::SH(String("ctrl_clmt_en_" + String(zone->id)).c_str()), zone->enabled);
        if (zone->enabled) {
            upd.update(su::SH(String("ctrl_clmt_sts_" + String(zone->id)).c_str()), zone->status);
            upd.update(su::SH(String("ctrl_clmt_name_" + String(zone->id)).c_str()), zone->name);
            upd.update(su::SH(("ctrl_clmt_sens_" + String(zone->id)).c_str()), (zone->sensor != nullptr) ? (String(zone->sensor->data.temp) + "°") : String("N/A"));
            upd.update(su::SH(("ctrl_clmt_temp_" + String(zone->id)).c_str()), zone->temp);
            upd.update(su::SH(("ctrl_clmt_dlt_" + String(zone->id)).c_str()), zone->delta);
            unsigned modeNum = static_cast<unsigned>(zone->type);
            upd.update(su::SH(("ctrl_clmt_mod_" + String(zone->id)).c_str()), modeNum);
            upd.update(su::SH(("ctrl_clmt_work_" + String(zone->id)).c_str()), zone->work);
        }
    }
}

void WebGUIClass::_buildSecurityPage(sets::Builder& b)
{

}

void WebGUIClass::_updateSecurityPage(sets::Updater& upd)
{

}

void WebGUIClass::_buildTankPage(sets::Builder& b)
{

}

void WebGUIClass::_updateTankPage(sets::Updater& upd)
{

}

WebGUIClass WebGUI;
