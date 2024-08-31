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

/*********************************************************************/
/*                                                                   */
/*                          PRIVATE FUNCTIONS                         */
/*                                                                   */
/*********************************************************************/

void TgBotClass::backMenu(TgUser *user)
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

bool TgBotClass::processLevel(TgUser *user, const String &msg)
{
    switch (user->level) {
        case TG_MENU_MAIN:
            {
                fb::Message resp;
                fb::Menu menu;

                if (msg == "Метео") {
                    user->level = TG_MENU_METEO;
                    return true;
                } else if (msg == "Камеры") {
                    user->level = TG_MENU_CAMS;
                    return true;
                } else if (msg == "Термо") {
                    user->level = TG_MENU_THERMS;
                    return true;
                } else if (msg == "Охрана") {
                    user->level = TG_MENU_SECURITY;
                    return true;
                } else if (msg == "Свет") {
                    user->level = TG_MENU_LIGHTS;
                    return true;
                } else if (msg == "Розетки") {
                    user->level = TG_MENU_SOCKETS;
                    return true;
                } else if (msg == "Бак") {
                    user->level = TG_MENU_TANKS;
                    return true;
                } else if (msg == "Полив") {
                    user->level = TG_MENU_WATERING;
                    return true;
                }

                resp.chatID = user->chatId;
                resp.text = F("Добро пожаловать в Умный Дом");

                menu.addButton("Я дома");
                menu.addButton("Ушёл");
                menu.newRow();
                menu.addButton("Камеры");
                menu.addButton("Метео");
                menu.addButton("Охрана");
                menu.addButton("Свет");
                menu.newRow();
                menu.addButton("Розетки");
                menu.addButton("Термо");
                menu.addButton("Бак");
                menu.addButton("Полив");

                resp.setMenu(menu);
                FastBot2::sendMessage(resp);
            }
            break;

        case TG_MENU_METEO:
            {
                fb::Message resp;
                fb::Menu menu;

                resp.chatID = user->chatId;
                resp.text = F("Метео");

                menu.addButton("Обновить");
                menu.addButton("Назад");

                resp.setMenu(menu);
                FastBot2::sendMessage(resp);
            }
            break;
    }
    return false;
}

void TgBotClass::updateHandler(fb::Update& upd)
{
    bool    changeLvl = false;

    auto id = upd.message().from().id().toInt32();
    auto user = getUser(id);

    if (user == nullptr) {
        fb::Message msg;
        msg.chatID = id;
        msg.text = F("Доступ запрещён");
        FastBot2::sendMessage(msg);
        return;
    }

    if (upd.message().text() == "Назад") {
        backMenu(user);
    }

    do {
        changeLvl = processLevel(user, upd.message().text());
    } while (changeLvl);
}

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

const std::vector<TgUser *> &TgBotClass::getUsers()
{
    return _users;
}

void TgBotClass::begin()
{
    if (!_enabled) return;

    Log.info(LOG_MOD_TG, "Starting Telegram Bot");

    FastBot2::attachUpdate([this](fb::Update& u){ updateHandler(u); });
    FastBot2::skipUpdates();
    FastBot2::begin();
}

void TgBotClass::loop()
{
    if (!_enabled) return;
    FastBot2::tick();
}

TgBotClass TgBot;
