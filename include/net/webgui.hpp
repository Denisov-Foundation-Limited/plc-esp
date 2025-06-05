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

typedef enum {
    WEB_PAGE_MAIN,
    WEB_PAGE_TELEGRAM,
    WEB_PAGE_CONTROLLERS,
    WEB_PAGE_SOCKETS,
    WEB_PAGE_SETTINGS,
    WEB_PAGE_SYSTEM,
    WEB_PAGE_METEO,
    WEB_PAGE_CLIMATE,
    WEB_PAGE_SECURITY,
    WEB_PAGE_TANK
} WebGuiPage;

typedef enum {
    WEB_GUI_MENU_BTN_NET = 1,
    WEB_GUI_MENU_BTN_TG,
    WEB_GUI_MENU_BTN_CTRL,
    WEB_GUI_MENU_BTN_CFG,
    WEB_GUI_SYS_SAVE,
    WEB_GUI_SYS_RESTART,
    WEB_GUI_SYS_DEL,
    WEB_GUI_SYS_TEMP,
    WEB_GUI_SYS_FAN_STATUS,
    WEB_GUI_SYS_FAN_EN,
    WEB_GUI_SYS_DATE,
    WEB_GUI_SYS_TIME,
    WEB_GUI_SYS_UTC,
    WEB_GUI_SYS_UTC_SET,
    WEB_GUI_SYS_UTC_APPLY,
    WEB_GUI_SYS_DTIME,
    WEB_GUI_MAIN_WIFI_EN,
    WEB_GUI_MAIN_WIFI_SSID,
    WEB_GUI_MAIN_WIFI_PWD,
    WEB_GUI_MAIN_WIFI_AP,
    WEB_GUI_MAIN_WIFI_IP,
    WEB_GUI_MAIN_WIFI_STATUS,
    WEB_GUI_CTRL_SOCKET,
    WEB_GUI_CTRL_METEO,
    WEB_GUI_CTRL_TANK,
    WEB_GUI_CTRL_SECURITY,
    WEB_GUI_CTRL_CLIMATE,
    WEB_GUI_CTRL_BACK,
    WEB_GUI_CTRL_SOCKET_ON_ALL,
    WEB_GUI_CTRL_SOCKET_OFF_ALL,
    WEB_GUI_CTRL_SOCKET_BACK,
    WEB_GUI_CTRL_SOCKET_SEL,
    WEB_GUI_CTRL_SOCKET_NAME,
    WEB_GUI_CTRL_SOCKET_ENABLE,
    WEB_GUI_CTRL_SOCKET_ENABLE_SOCK,
    WEB_GUI_CTRL_SOCKET_RLY,
    WEB_GUI_CTRL_SOCKET_BTN,
    WEB_GUI_CTRL_SOCKET_LED,
    WEB_GUI_TG_EN,
    WEB_GUI_TG_PERIOD,
    WEB_GUI_TG_POLL,
    WEB_GUI_TG_TOKEN,
    WEB_GUI_TG_LAST_ID,
    WEB_GUI_TG_USER_NAME,
    WEB_GUI_TG_USER_EN,
    WEB_GUI_TG_USER_ADMIN,
    WEB_GUI_TG_USER_NTF,
    WEB_GUI_TG_USER_CHATID,
    WEB_GUI_CTRL_METEO_SEL,
    WEB_GUI_CTRL_METEO_NAME,
    WEB_GUI_CTRL_METEO_ENABLE,
    WEB_GUI_CTRL_METEO_ENABLE_SENS,
    WEB_GUI_CTRL_METEO_TYPE,
    WEB_GUI_CTRL_METEO_ADDR,
    WEB_GUI_CTRL_CLIMATE_ENABLE
} WebGUIElem;

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
