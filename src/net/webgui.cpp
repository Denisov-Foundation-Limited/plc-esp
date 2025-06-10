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

#include "controllers/security.hpp"
#include "controllers/tank.hpp"
#include "core/clock.hpp"
#include "net/pages/elements.hpp"
#include "net/pages/climatep.hpp"
#include "net/pages/meteop.hpp"
#include "net/pages/socketp.hpp"

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
                _curPage = SocketPage.build(b);
                break;
            case WEB_PAGE_METEO:
                _curPage = MeteoPage.build(b);
                break;
            case WEB_PAGE_CLIMATE:
                _curPage = ClimatePage.build(b);
                break;
            case WEB_PAGE_SECURITY:
                break;
            case WEB_PAGE_TANK:
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
                SocketPage.update(upd);
                break;
            case WEB_PAGE_SETTINGS:
                _updateSettingsPage(upd);
                break;
            case WEB_PAGE_METEO:
                MeteoPage.update(upd);
                break;
            case WEB_PAGE_CLIMATE:
                ClimatePage.update(upd);
                break;
            case WEB_PAGE_SECURITY:
                break;
            case WEB_PAGE_TANK:
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
        if (b.Button(WEB_GUI_CTRL_SOCKET, F("Розетки"))) {
            _curPage = WEB_PAGE_SOCKETS;
            b.reload();
        }
        if (b.Button(WEB_GUI_CTRL_METEO, F("Метео"))) {
            _curPage = WEB_PAGE_METEO;
            b.reload();
        }
        if (b.Button(WEB_GUI_CTRL_CLIMATE, F("Климат"))) {
            _curPage = WEB_PAGE_CLIMATE;
            b.reload();
        }
        if (b.Button(WEB_GUI_CTRL_SECURITY, F("Охрана"))) {
            _curPage = WEB_PAGE_SECURITY;
            b.reload();
        }
        if (b.Button(WEB_GUI_CTRL_TANK, F("Баки"))) {
            _curPage = WEB_PAGE_TANK;
            b.reload();
        }
        if (b.Button(WEB_GUI_CTRL_BACK, F("Назад"), sets::Colors::Aqua)) {
            _curPage = WEB_PAGE_MAIN;
            b.reload();
        }
        b.endGroup();
    }
}

void WebGUIClass::_updateCtrlsPage(sets::Updater& upd)
{
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

WebGUIClass WebGUI;
