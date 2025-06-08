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

#ifndef __WEB_GUI_HPP__
#define __WEB_GUI_HPP__

#include <Arduino.h>
#include <SettingsAsync.h>

#include "net/tgbot.hpp"
#include "controllers/ctrls.hpp"
#include "controllers/ctrl.hpp"
#include "pages/pages.hpp"

class WebGUIClass : public SettingsAsync
{
public:
    void begin();
    void loop();
    void setPassword(const String &pwd);
    const String &getPassword();

private:
    String      _password = "";
    WebGuiPage  _curPage = WEB_PAGE_MAIN;

    unsigned _time;

    struct {
        String      Name;
        unsigned    curUser;
        bool        Enabled;
        bool        Admin;
        bool        Notify;
        unsigned    ChatID;
    } _tgUser;

    struct {
        String  Name;
        bool    Enabled;
        size_t  curRly = 0;
        size_t  curBtn = 0;
        size_t  curSock = 0;
        size_t  curLED = 0;
    } _socket;

    struct {
        size_t  curSensor = 0;
    } _meteo;

    void _buildMenu(sets::Builder& b);
    void _updateMainPage(sets::Updater& upd);
    void _buildMainPage(sets::Builder& b);
    void _buildTgBotPage(sets::Builder& b);
    void _updateTgBotPage(sets::Updater& upd);
    void _buildCtrlsPage(sets::Builder& b);
    void _updateCtrlsPage(sets::Updater& upd);
    void _buildSettingsPage(sets::Builder& b);
    void _updateSettingsPage(sets::Updater& upd);
    void _buildSocketsPage(sets::Builder& b);
    void _updateSocketsPage(sets::Updater& upd);
    void _buildMeteoPage(sets::Builder& b);
    void _updateMeteoPage(sets::Updater& upd);
    void _buildClimatePage(sets::Builder& b);
    void _updateClimatePage(sets::Updater& upd);
    void _buildSecurityPage(sets::Builder& b);
    void _updateSecurityPage(sets::Updater& upd);
    void _buildTankPage(sets::Builder& b);
    void _updateTankPage(sets::Updater& upd);
};

extern WebGUIClass WebGUI;

#endif /* __WEB_GUI_HPP__ */
