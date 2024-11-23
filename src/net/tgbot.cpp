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

#include "net/tgbot.hpp"
#include "net/core/wifi.hpp"
#include "net/core/eth.hpp"
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

bool TgBotClass::getEnabled() const
{
    return _enabled;
}

void TgBotClass::addUser(TgUser *user)
{
    _users.push_back(user);
}

TgUser *TgBotClass::getUser(const String &name)
{
    for (auto u : _users) {
        if (u->name == name) {
            return u;
        }
    }
    return nullptr;
}

TgUser *TgBotClass::getUser(unsigned chatId)
{
    for (auto u : _users) {
        if (u->chatId == chatId) {
            return u;
        }
    }
    return nullptr;
}

bool TgBotClass::isUserExists(const String &name) const
{
    for (auto u : _users) {
        if (u->name == name) {
            return true;
        }
    }
    return false;
}

const std::vector<TgUser *> &TgBotClass::getUsers()
{
    return _users;
}

void TgBotClass::begin()
{
    if (!_enabled || getToken() == "") return;

    Log.info(LOG_MOD_TG, "Starting Telegram Bot");

    attachUpdate([this](fb::Update& u){ _updateHandler(u); });
    skipUpdates();
    FastBot2::begin();
}

void TgBotClass::loop()
{
    if (!_enabled || getToken() == "") return;
    if (Wireless.getEnabled() && Wireless.getStatus() != WL_CONNECTED) return;
    if (EthernetCard.getEnabled() && !EthernetCard.getStatus()) return;
    tick();
}

void TgBotClass::removeUser(size_t idx)
{
    delete _users[idx];
    _users.erase(_users.cbegin() + idx);
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

    auto id = upd.message().from().id().toInt32();
    auto user = getUser(id);
    auto msg = upd.message().text().decodeUnicode();

    _lastID = id;

    if (user == nullptr) {
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

    auto *ctrl = Controllers.getController(msg);
    if (ctrl != nullptr) {
        user->ctrl = ctrl->getName();

        switch (ctrl->getType()) {
            case CTRL_TYPE_SOCKET:
                user->level = TG_MENU_SOCKETS;
                return true;
        }
    }

    resp.chatID = user->chatId;
    resp.mode = fb::Message::Mode::HTML;
    resp.text = F("<b>Добро пожаловать в Город Будущего</b>");

    menu.addButton(F("Я дома"));
    menu.addButton(F("Ушёл"));
    menu.newRow();

    size_t i = 0;
    for (auto *ctrl : Controllers.getControllers()) {
        i++;
        menu.addButton(ctrl->getName());
        if (i % 4 == 0) {
            menu.newRow();
        }
    }

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
    fb::Message resp;
    fb::Menu    menu;

    resp.chatID = user->chatId;
    resp.mode = fb::Message::Mode::HTML;

    auto ctrl = static_cast<SocketCtrl *>(Controllers.getController(user->ctrl));
    if (ctrl == nullptr) {
        user->level = TG_MENU_MAIN;
        resp.text = F("<b>Ошибка:</b> Контроллер не найден");
        sendMessage(resp);
        return false;
    }

    resp.text.concat(F("<b>РОЗЕТКИ: "));
    resp.text.concat(ctrl->getName());
    resp.text.concat(F("</b>\n\n"));

    menu.addButton(F("Назад"));
    menu.addButton(F("Статус"));
    menu.addButton(F("Вкл.все"));
    menu.addButton(F("Откл.все"));
    menu.newRow();

    for (auto *socket : ctrl->getSockets()) {
        if (msg == socket->getName()) {
            socket->setStatus(!socket->getStatus(), true);
        } else if (msg == "Вкл.все") {
            socket->setStatus(true, true);
        } else if (msg == "Откл.все") {
            socket->setStatus(false, true);
        }
    }

    for (auto *socket : ctrl->getSockets()) {
        resp.text.concat("<b>");
        resp.text.concat(socket->getName());
        resp.text.concat(":</b> ");
        resp.text.concat((socket->getStatus() ? F("Включен") : F("Отключен")));
        resp.text.concat("\n");
        menu.addButton(socket->getName());
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
