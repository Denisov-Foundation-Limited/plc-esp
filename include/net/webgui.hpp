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

#include "net/tgbot.hpp"
#include "controllers/ctrls.hpp"
#include "controllers/ctrl.hpp"

typedef enum {
    WEB_PAGE_MAIN,
    WEB_PAGE_TELEGRAM,
    WEB_PAGE_CONTROLLERS,
    WEB_PAGE_SOCKETS,
    WEB_PAGE_SETTINGS,
    WEB_PAGE_SYSTEM
} WebGuiPage;

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

    struct {
        String      Name;
        String      Error;
        unsigned    curUser;
    } _tgUser;

    struct {
        String      Name;
        CtrlType    Type = CTRL_TYPE_SOCKET;
        String      Error;
        unsigned    curCtrl = 0;
    } _ctrl;

    struct {
        String  Name;
        String  Error;
        size_t  curRly = 0;
        size_t  curBtn = 0;
        size_t  curSock = 0;
    } _socket;

    void _buildMenu(sets::Builder& b);
    void _updateMainPage(sets::Updater& upd);
    void _buildMainPage(sets::Builder& b);
    void _buildTgBotPage(sets::Builder& b);
    void _updateTgBotPage(sets::Updater& upd);
    void _buildCtrlsPage(sets::Builder& b);
    void _updateCtrlsPage(sets::Updater& upd);
    void _buildSettingsPage(sets::Builder& b);

    void _buildSocketsPage(sets::Builder& b);
    void _updateSocketsPage(sets::Updater& upd);
};

extern WebGUIClass WebGUI;

#endif /* __WEB_GUI_HPP__ */
