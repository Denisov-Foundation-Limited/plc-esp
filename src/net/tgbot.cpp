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

#include "net/tgbot.hpp"
#include "net/core/wifi.hpp"
#include "controllers/ctrls.hpp"
#include "controllers/socket/socket.hpp"

/*********************************************************************/
/*                                                                   */
/*                          PUBLIC FUNCTIONS                         */
/*                                                                   */
/*********************************************************************/

void TgBotClass::setEnabled(bool status)
{
    _enabled = status;
}

bool &TgBotClass::getEnabled()
{
    return _enabled;
}

bool TgBotClass::getUser(const String &name, TgUser **user)
{
    for (size_t i = 0; i < _users.size(); i++) {
        if (_users[i].name == name) {
            *user = &_users[i];
            return true;
        }
    }
    return false;
}

bool TgBotClass::getUser(size_t index, TgUser **user)
{
    for (size_t i = 0; i < _users.size(); i++) {
        if (i == index) {
            *user = &_users[i];
            return true;
        }
    }
    return false;
}

bool TgBotClass::getUserByChatId(unsigned chatId, TgUser **user)
{
    for (size_t i = 0; i < _users.size(); i++) {
        if (_users[i].chatId == chatId) {
            *user = &_users[i];
            return true;
        }
    }
    return false;
}

bool TgBotClass::isUserExists(const String &name) const
{
    for (auto u : _users) {
        if (u.name == name) {
            return true;
        }
    }
    return false;
}

bool TgBotClass::setUser(size_t index, TgUser *user)
{
    if (index >= _users.size())
        return false;

    memcpy(&_users[index], user, sizeof(TgUser));

    return true;
}

void TgBotClass::getEnabledUsers(std::vector<TgUser *> &users)
{
    for (size_t i = 0; i < _users.size(); i++) {
        if (_users[i].enabled) {
            users.push_back(&_users[i]);
        }
    }
}

std::array<TgUser, TG_USERS_COUNT> *TgBotClass::getUsers()
{
    return &_users;
}

void TgBotClass::begin()
{
    if (!_enabled || getToken() == "") return;

    for (auto user : _users) {
        user.level = TG_MENU_MAIN;
    }

    Log.info(F("TG"), "Starting Telegram Bot");

    attachUpdate([this](fb::Update& u){ _updateHandler(u); });
    skipUpdates();
    FastBot2::begin();
}

void TgBotClass::loop()
{
    if (!_enabled || getToken() == "") return;
    if (Wireless.getEnabled() && Wireless.getStatus() != WL_CONNECTED) return;
    tick();
}

unsigned TgBotClass::getLastID() const
{
    return _lastID;
}

/*********************************************************************/
/*                                                                   */
/*                          PRIVATE FUNCTIONS                         */
/*                                                                   */
/*********************************************************************/

void TgBotClass::_backMenu(TgUser *user)
{
    switch (user->level) {
        case TG_MENU_MAIN:
        case TG_MENU_METEO:
        case TG_MENU_SECURITY:
        case TG_MENU_TANKS:
        case TG_MENU_SOCKETS:
        case TG_MENU_LIGHTS:
        case TG_MENU_WATERING:
        case TG_MENU_THERMS:
        case TG_MENU_CAMS:
            user->level = TG_MENU_MAIN;
            break;

        case TG_MENU_TANK:
            user->level = TG_MENU_TANKS;
            break;

        case TG_MENU_WATERER:
            user->level = TG_MENU_WATERING;
            break;

        case TG_MENU_THERM:
            user->level = TG_MENU_THERMS;
            break;
    }
}

bool TgBotClass::_processLevel(TgUser *user, const String &msg)
{
    switch (user->level) {
        case TG_MENU_MAIN:
            return _mainHandler(user, msg);

        case TG_MENU_METEO:
            return _meteoHandler(user, msg);

        case TG_MENU_SECURITY:
            return _securityHandler(user, msg);

        case TG_MENU_CAMS:
            return _camsHandler(user, msg);

        case TG_MENU_TANKS:
            return _tanksHandler(user, msg);

        case TG_MENU_THERMS:
            return _thermsHandler(user, msg);

        case TG_MENU_LIGHTS:
            return _lightsHandler(user, msg);

        case TG_MENU_SOCKETS:
            return _socketsHandler(user, msg);

        case TG_MENU_WATERING:
            return _wateringHandler(user, msg);
    }
    return false;
}

void TgBotClass::_updateHandler(fb::Update& upd)
{
    bool    changeLvl = false;
    TgUser  *user;

    auto id = upd.message().from().id().toInt32();
    auto msg = upd.message().text().decodeUnicode();

    _lastID = id;

    if (!getUserByChatId(id, &user)) {
        fb::Message msg;
        msg.chatID = id;
        msg.text = F("Доступ запрещён");
        sendMessage(msg);
        return;
    }

    if (msg == "Назад") {
        _backMenu(user);
    }

    do {
        changeLvl = _processLevel(user, msg);
    } while (changeLvl);
}

bool TgBotClass::_mainHandler(TgUser *user, const String &msg)
{
    fb::Message resp;
    fb::Menu    menu;

    resp.chatID = user->chatId;
    resp.mode = fb::Message::Mode::HTML;
    resp.text = F("<b>Добро пожаловать в Город Будущего</b>");

    if (msg == "Розетки") {
        user->level = TG_MENU_SOCKETS;
        return true;
    }

    menu.addButton(F("Я дома"));
    menu.addButton(F("Ушёл"));
    menu.newRow();

    menu.addButton("Розетки");
    menu.newRow();

    resp.setMenu(menu);
    sendMessage(resp);

    return false;
}

bool TgBotClass::_meteoHandler(TgUser *user, const String &msg)
{
    fb::Message resp;
    fb::Menu    menu;

    resp.chatID = user->chatId;
    resp.mode = fb::Message::Mode::HTML;
    resp.text = F("<b>Метео</b>");

    menu.addButton(F("Обновить"));
    menu.addButton(F("Назад"));

    resp.setMenu(menu);
    sendMessage(resp);
    return false;
}

bool TgBotClass::_securityHandler(TgUser *user, const String &msg)
{
    fb::Message resp;
    fb::Menu    menu;

    resp.chatID = user->chatId;
    resp.mode = fb::Message::Mode::HTML;
    resp.text = F("<b>Охрана</b>");

    menu.addButton(F("Обновить"));
    menu.addButton(F("Назад"));

    resp.setMenu(menu);
    sendMessage(resp);
    return false;
}

bool TgBotClass::_camsHandler(TgUser *user, const String &msg)
{
    fb::Message resp;
    fb::Menu    menu;

    resp.chatID = user->chatId;
    resp.mode = fb::Message::Mode::HTML;
    resp.text = F("<b>Камеры</b>");

    menu.addButton(F("Обновить"));
    menu.addButton(F("Назад"));

    resp.setMenu(menu);
    sendMessage(resp);
    return false;
}

bool TgBotClass::_tanksHandler(TgUser *user, const String &msg)
{
    fb::Message resp;
    fb::Menu    menu;

    resp.chatID = user->chatId;
    resp.mode = fb::Message::Mode::HTML;
    resp.text = F("<b>Баки</b>");

    menu.addButton(F("Обновить"));
    menu.addButton(F("Назад"));

    resp.setMenu(menu);
    sendMessage(resp);
    return false;
}

bool TgBotClass::_socketsHandler(TgUser *user, const String &msg)
{
    fb::Message             resp;
    fb::Menu                menu;
    std::vector<Socket *>   socks;

    resp.chatID = user->chatId;
    resp.mode = fb::Message::Mode::HTML;

    resp.text.concat(F("<b>РОЗЕТКИ:</b>\n\n"));

    menu.addButton(F("Назад"));
    menu.addButton(F("Статус"));
    menu.addButton(F("Вкл.все"));
    menu.addButton(F("Откл.все"));
    menu.newRow();

    SocketCtrl.getEnabledSockets(socks);

    for (auto *socket : socks) {
        if (msg == socket->name) {
            SocketCtrl.setStatus(socket, !socket->status, true);
        } else if (msg == "Вкл.все") {
            SocketCtrl.setStatus(socket, true, true);
        } else if (msg == "Откл.все") {
            SocketCtrl.setStatus(socket, false, true);
        }
    }

    for (auto *socket : socks) {
        resp.text.concat("<b>");
        resp.text.concat(socket->name);
        resp.text.concat(":</b> ");
        resp.text.concat((socket->status ? F("Включен") : F("Отключен")));
        resp.text.concat("\n");
        menu.addButton(socket->name);
        menu.newRow();
    }

    resp.setMenu(menu);
    sendMessage(resp);

    return false;
}

bool TgBotClass::_lightsHandler(TgUser *user, const String &msg)
{
    fb::Message resp;
    fb::Menu    menu;

    resp.chatID = user->chatId;
    resp.mode = fb::Message::Mode::HTML;
    resp.text = F("<b>Полив</b>");

    menu.addButton(F("Обновить"));
    menu.addButton(F("Назад"));

    resp.setMenu(menu);
    auto res = sendMessage(resp);
    return false;
}

bool TgBotClass::_wateringHandler(TgUser *user, const String &msg)
{
    fb::Message resp;
    fb::Menu    menu;

    resp.chatID = user->chatId;
    resp.mode = fb::Message::Mode::HTML;
    resp.text = F("<b>Полив</b>");

    menu.addButton(F("Обновить"));
    menu.addButton(F("Назад"));

    resp.setMenu(menu);
    auto res = sendMessage(resp);
    return false;
}

bool TgBotClass::_thermsHandler(TgUser *user, const String &msg)
{
    fb::Message resp;
    fb::Menu    menu;

    resp.chatID = user->chatId;
    resp.mode = fb::Message::Mode::HTML;
    resp.text = F("<b>Термоконтроль</b>");

    menu.addButton(F("Обновить"));
    menu.addButton(F("Назад"));

    resp.setMenu(menu);
    sendMessage(resp);
    return false;
}

TgBotClass TgBot;
