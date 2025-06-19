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

#include "net/core/wifi.hpp"

#include "net/pages/elements.hpp"
#include "net/pages/climatep.hpp"
#include "net/pages/meteop.hpp"
#include "net/pages/socketp.hpp"
#include "net/pages/tgbotp.hpp"
#include "net/pages/settingsp.hpp"
#include "net/pages/securityp.hpp"

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
                _curPage = TgbotPage.build(b);
                break;
            case WEB_PAGE_CONTROLLERS:
                _buildCtrlsPage(b);
                break;
            case WEB_PAGE_SETTINGS:
                SettingsPage.build(b);
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
                _curPage = SecurityPage.build(b);
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
                TgbotPage.update(upd);
                break;
            case WEB_PAGE_CONTROLLERS:
                _updateCtrlsPage(upd);
                break;
            case WEB_PAGE_SOCKETS:
                SocketPage.update(upd);
                break;
            case WEB_PAGE_SETTINGS:
                SettingsPage.update(upd);
                break;
            case WEB_PAGE_METEO:
                MeteoPage.update(upd);
                break;
            case WEB_PAGE_CLIMATE:
                ClimatePage.update(upd);
                break;
            case WEB_PAGE_SECURITY:
                SecurityPage.update(upd);
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

WebGUIClass WebGUI;
