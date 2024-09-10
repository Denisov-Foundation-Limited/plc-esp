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

#ifndef __TG_BOT_HPP__
#define __TG_BOT_HPP__

#include <FastBot2.h>
#include <Arduino.h>
#include "vector"
#include <functional>

#include <WiFiClient.h>
#include <WiFiServer.h>
#include <GyverHTTP.h>

#include "utils/log.hpp"

typedef enum {
    TG_MENU_MAIN,
    TG_MENU_METEO,
    TG_MENU_SECURITY,
    TG_MENU_TANKS,
    TG_MENU_TANK,
    TG_MENU_SOCKETS,
    TG_MENU_LIGHTS,
    TG_MENU_CAMS,
    TG_MENU_WATERING,
    TG_MENU_WATERER,
    TG_MENU_THERMS,
    TG_MENU_THERM
} TgMenuLevel;

typedef struct {
    TgMenuLevel level;
    bool        read;
    bool        write;
} TgUserRight;

typedef struct {
    String      name;
    unsigned    chatId;
    bool        notify;
    bool        admin;
    TgMenuLevel level;
} TgUser;

class TgBotClass : public FastBot2
{
public:
    void setEnabled(bool status);
    bool getEnabled() const;
    void addUser(TgUser *user);
    TgUser *getUser(const String &name);
    TgUser *getUser(unsigned chatId);
    bool removeUser(const String &name);
    const std::vector<TgUser *> &getUsers();
    void begin();
    void loop();

private:
    std::vector<TgUser *>   _users;
    bool                    _enabled = false;

    void _backMenu(TgUser *user);
    bool _processLevel(TgUser *user, const String &msg);
    void _updateHandler(fb::Update& upd);

    bool _mainHandler(TgUser *user, const String &msg);
    bool _meteoHandler(TgUser *user, const String &msg);
    bool _securityHandler(TgUser *user, const String &msg);
    bool _camsHandler(TgUser *user, const String &msg);
    bool _tanksHandler(TgUser *user, const String &msg);
    bool _socketsHandler(TgUser *user, const String &msg);
    bool _lightsHandler(TgUser *user, const String &msg);
    bool _wateringHandler(TgUser *user, const String &msg);
    bool _thermsHandler(TgUser *user, const String &msg);
};

extern TgBotClass TgBot;

#endif /* __TG_BOT_HPP__ */
