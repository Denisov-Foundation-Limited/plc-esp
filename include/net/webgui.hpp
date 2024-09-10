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

#ifndef __WEB_GUI_HPP__
#define __WEB_GUI_HPP__

#include <Arduino.h>
#include <SettingsAsync.h>
#include "net/wifi.hpp"
#include "net/tgbot.hpp"

typedef enum {
    WEB_PAGE_MAIN,
    WEB_PAGE_TELEGRAM,
    WEB_PAGE_CONTROLLERS,
    WEB_PAGE_SETTINGS
} WebGuiPage;

class WebGUIClass final : private SettingsAsync
{
public:
    void begin();
    void loop();
    void setPassword(const String &pwd);
    const String &getPassword();

private:
    String      _password = "";
    WebGuiPage  _curPage = WEB_PAGE_MAIN;
    TgUser      _newTgUser;

    void _buildMenu(sets::Builder& b);
    void _buildMainPage(sets::Builder& b);
    void _buildTgBotPage(sets::Builder& b);
    void _updateTgBotPage(sets::Updater& upd);
    void _buildCtrlsPage(sets::Builder& b);
    void _buildSettingsPage(sets::Builder& b);
};

extern WebGUIClass WebGUI;

#endif /* __WEB_GUI_HPP__ */
