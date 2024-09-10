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

#include "net/webgui.hpp"
#include "utils/configs.hpp"

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
        }
    });

    onUpdate([this](sets::Updater& upd) {
        switch (_curPage) {
            case WEB_PAGE_MAIN:
                break;
            case WEB_PAGE_TELEGRAM:
                _updateTgBotPage(upd);
                break;
            case WEB_PAGE_CONTROLLERS:
                break;
            case WEB_PAGE_SETTINGS:
                break;
        }
    });

    setTitle(F("Future City PLC"));
    setPass(_password);
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
            if (b.Button("menu_btn_net"_h, F("Сеть"))) {
            _curPage = WEB_PAGE_MAIN;
            b.reload();
        }
        if (b.Button("menu_btn_tg"_h, F("Telegram"))) {
            _curPage = WEB_PAGE_TELEGRAM;
            b.reload();
        }
        if (b.Button("menu_btn_ctrl"_h, F("Контроллеры"))) {
            _curPage = WEB_PAGE_CONTROLLERS;
            b.reload();
        }
        if (b.Button("menu_btn_set"_h, F("Настройки"))) {
            _curPage = WEB_PAGE_SETTINGS;
            b.reload();
        }
        b.endButtons();
        b.endGroup();
    }
}

void WebGUIClass::_buildMainPage(sets::Builder& b)
{
    if (b.beginGroup(F("Ethernet"))) {
        if (b.Switch("main_eth_en"_h, F("Enabled"), String(true))) {

        }
        b.LED(F("Connection"), String(true));
        if (b.Input("main_eth_ip"_h, F("IP"), "192.168.0.12")) {

        }
        if (b.Input("main_eth_gw"_h, F("Gateway"), "192.168.0.1")) {
            
        }
        b.endGroup();
    }

    if (b.beginGroup(F("Wi-Fi"))) {
        if (b.Switch("main_wf_en"_h, F("Enabled"), String(Wireless.getEnabled()))) {
            Wireless.setEnabled(b.build().value().toBool());
            b.reload();
        }
        if (Wireless.getEnabled()) {
            if (b.Input("main_wf_ssid"_h, F("SSID"), Wireless.getSSID())) {
                Wireless.setCreds(b.build().value(), Wireless.getPasswd());
            }
            if (b.Pass("main_wf_pwd"_h, F("Password"), Wireless.getPasswd())) {
                Wireless.setCreds(Wireless.getSSID(), b.build().value());
            }
            if (b.Switch("main_wf_ap"_h, F("AP"), String(Wireless.getAP()))) {
                Wireless.setAP(b.build().value().toBool());
            }
            b.LED(F("Connection"), String(Wireless.getStatus() == WL_CONNECTED));
            b.Label(F("IP"), Wireless.getIP());
        }
        b.endGroup();
    }

    b.Label(F("Copyright"), F("Denisov Foundation Limited 2024"));
}

void WebGUIClass::_buildTgBotPage(sets::Builder& b)
{
    if (b.beginGroup(F("Telegram"))) {
        if (b.Switch("tg_en"_h, F("Enabled"), String(TgBot.getEnabled()))) {
            TgBot.setEnabled(b.build().value().toBool());
            b.reload();
        }
        if (TgBot.getEnabled()) {
            if (b.Pass("tg_key"_h, F("Token"), TgBot.getToken())) {
                TgBot.setToken(b.build().value());
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
            if (b.Select("tg_mod"_h, F("Mode"), F("Long;Sync;Async"), String(modeNum))) {
                switch (b.build().value().toInt32()) {
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
            }
            if (b.Slider("tg_period"_h, "Period", 100, 100000, 10, F("msec"), String(TgBot.getPollPeriod()))) {
                TgBot.setPollMode(TgBot.getPollMode(), b.build().value().toInt32());
            }
        }
        b.endGroup();

        if (b.beginMenu("Пользователи")) {
            if (b.beginGroup(F("Выбрать"))) {
                String users = "";
                size_t last = TgBot.getUsers().size() - 1;
                size_t i = 0;
                for (auto *user : TgBot.getUsers()) {
                    users += user->name;
                    if (i < last) {
                        users += ";";
                    }
                    i++;
                }
                if (b.Select("tg_susr"_h, F("Name"), users, String(0))) {
                    auto *user = TgBot.getUsers()[b.build().value().toInt32()];
                    if (user != nullptr) {
                        _newTgUser.name = user->name;
                        _newTgUser.chatId = user->chatId;
                        _newTgUser.notify = user->notify;
                        _newTgUser.admin = user->admin;
                    }
                }
                b.endGroup();
            }

            if (b.beginGroup(F("Редактировать"))) {
                if (b.Input("tg_nusr_name"_h, F("Name"), _newTgUser.name)) {
                    _newTgUser.name = b.build().value();
                }
                if (b.Input("tg_nusr_id"_h, F("ChatID"), String(_newTgUser.chatId))) {
                    _newTgUser.chatId = b.build().value().toInt32();
                }
                if (b.Switch("tg_nusr_ntf"_h, F("Notify"), String(_newTgUser.notify))) {
                    _newTgUser.notify = b.build().value().toBool();
                }
                if (b.Switch("tg_nusr_adm"_h, F("Admin"), String(_newTgUser.admin))) {
                    _newTgUser.admin = b.build().value().toBool();
                }
                b.beginButtons();
                if (b.Button("tg_nusr_add"_h, F("Добавить"))) {
                    auto user = new TgUser();
                    user->name = _newTgUser.name;
                    user->chatId = _newTgUser.chatId;
                    user->notify = _newTgUser.notify;
                    user->admin = _newTgUser.admin;
                    user->level = TG_MENU_MAIN;
                    TgBot.addUser(user);
                    _newTgUser.name = "";
                    _newTgUser.notify = false;
                    _newTgUser.admin = false;
                    _newTgUser.chatId = 0;
                    b.reload();
                }
                if (b.Button("tg_nusr_upd"_h, F("Обновить"), sets::Colors::Aqua)) {
                    auto user = TgBot.getUser(_newTgUser.name);
                    if (user != nullptr) {
                        user->name = _newTgUser.name;
                        user->chatId = _newTgUser.chatId;
                        user->notify = _newTgUser.notify;
                        user->admin = _newTgUser.admin;
                    }
                    b.reload();
                }
                if (b.Button("tg_nusr_rm"_h, F("Удалить"), sets::Colors::Red)) {
                    TgBot.removeUser(_newTgUser.name);
                }
                b.endButtons();
                b.endGroup();

                size_t i = 1;
                for (auto *user : TgBot.getUsers()) {
                    if (b.beginGroup("Пользователь #" + String(i))) {
                        b.Label(F("Name"), user->name);
                        b.Label(F("ChatID"), String(user->chatId));
                        b.Switch("tg_usr_ntf"_h, F("Notify"), String(user->notify));
                        b.Switch("tg_usr_adm"_h, F("Admin"), String(user->admin));
                        b.endGroup();
                    }
                    i++;
                }
            }
            b.endMenu();
        }
    }
}

void WebGUIClass::_updateTgBotPage(sets::Updater& upd)
{
    upd.update("tg_nusr_name"_h, _newTgUser.name);
    upd.update("tg_nusr_id"_h, String(_newTgUser.chatId));
    upd.update("tg_nusr_ntf"_h, String(_newTgUser.notify));
    upd.update("tg_nusr_adm"_h, String(_newTgUser.admin));
}

void WebGUIClass::_buildCtrlsPage(sets::Builder& b)
{
    
}

void WebGUIClass::_buildSettingsPage(sets::Builder& b)
{
    if (b.beginGroup(F("Настройки"))) {
        b.beginButtons();
        if (b.Button("set_sv"_h, F("Сохранить"))) {
            Configs.writeAll();
            b.reload();
        }
        if (b.Button("set_rm"_h, F("Удалить"), sets::Colors::Red)) {
            Configs.eraseAll();
            b.reload();
        }
        b.endButtons();
        b.endGroup();
    }

    if (b.beginGroup(F("Система"))) {
        if (b.Button("set_res"_h, F("Рестарт"))) {
            ESP.restart();
        }
        b.endGroup();
    }
}

WebGUIClass WebGUI;
