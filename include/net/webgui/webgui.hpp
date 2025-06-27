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

#include "controllers/ctrls.hpp"
#include "pages/pages.hpp"
#include "net/webgui/pages/elements.hpp"
#include "net/webgui/pages/climatep.hpp"
#include "net/webgui/pages/meteop.hpp"
#include "net/webgui/pages/socketp.hpp"
#include "net/webgui/pages/tgbotp.hpp"
#include "net/webgui/pages/settingsp.hpp"
#include "net/webgui/pages/securityp.hpp"
#include "net/webgui/pages/tankp.hpp"
#include "net/webgui/pages/stackp.hpp"

class WebGUIClass : public SettingsAsync
{
public:
    void begin();
    void loop();
    void setPassword(const String &pwd);
    const String &getPassword();

private:
    String          _password = "";
    WebGuiPage      _curPage = WEB_PAGE_MAIN;
    unsigned        _time;
    ClimatePage     _climate;
    MeteoPage       _meteo;
    SecurityPage    _security;
    SettingsPage    _settings;
    SocketPage      _socket;
    StackPage       _stack;
    TankPage        _tank;
    TgbotPage       _tgbot;

    void _buildMenu(sets::Builder& b);
    void _updateMainPage(sets::Updater& upd);
    void _buildMainPage(sets::Builder& b);
    void _buildCtrlsPage(sets::Builder& b);
    void _updateCtrlsPage(sets::Updater& upd);
};

extern WebGUIClass WebGUI;

#endif /* __WEB_GUI_HPP__ */
