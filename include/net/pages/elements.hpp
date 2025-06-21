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

#ifndef __WEB_ELEMENTS_HPP__
#define __WEB_ELEMENTS_HPP__

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
    WEB_GUI_SYS_UTC_SET,
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
    WEB_GUI_CTRL_SOCKET_ENABLE,
    WEB_GUI_CTRL_SECURITY_ALARM,
    WEB_GUI_CTRL_SECURITY_STATUS,
    WEB_GUI_TG_EN,
    WEB_GUI_TG_PERIOD,
    WEB_GUI_TG_POLL,
    WEB_GUI_TG_TOKEN,
    WEB_GUI_TG_LAST_ID,
    WEB_GUI_CTRL_METEO_ENABLE,
    WEB_GUI_CTRL_CLIMATE_ENABLE
} WebGUIElem;

#endif /* __WEB_ELEMENTS_HPP__ */
