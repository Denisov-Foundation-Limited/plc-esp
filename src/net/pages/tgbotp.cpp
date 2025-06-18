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

#include "net/pages/tgbotp.hpp"
#include "net/pages/elements.hpp"
#include "net/tgbot.hpp"

/*********************************************************************/
/*                                                                   */
/*                          PUBLIC FUNCTIONS                         */
/*                                                                   */
/*********************************************************************/

WebGuiPage TgbotPageClass::build(sets::Builder& b)
{
    std::vector<TgUser *>   users;
    WebGuiPage              curPage = WEB_PAGE_TELEGRAM;

    TgBot.getUsers(false, users);
    
    if (b.beginGroup(F("Телеграм"))) {
        if (b.Switch(WEB_GUI_TG_EN, F("Включен"), &TgBot.getEnabled())) {
            b.reload();
        }
        if (TgBot.getEnabled()) {
            String token = TgBot.getToken();
            if (b.Pass(WEB_GUI_TG_TOKEN, F("Токен"), &token)) {
                TgBot.setToken(b.build.value);
                TgBot.begin();
            }
            uint8_t poll = static_cast<uint8_t>(TgBot.getPollMode());
            if (b.Select(WEB_GUI_TG_POLL, F("Режим"), F("Sync;Async;Long"), &poll)) {
                TgBot.setPollMode(static_cast<fb::Poll>(b.build.value.toInt32()), TgBot.getPollPeriod());
                TgBot.begin();
            }
            uint16_t period = TgBot.getPollPeriod();
            if (b.Slider(WEB_GUI_TG_PERIOD, "Период", 100, 100000, 10, F("msec"), &period)) {
                TgBot.setPollMode(TgBot.getPollMode(), b.build.value.toInt32());
                TgBot.begin();
            }
            b.Number(WEB_GUI_TG_LAST_ID, F("ChatID"), &TgBot.getLastID());
        }
        if (b.Button(F("Назад"), sets::Colors::Aqua)) {
            curPage = WEB_PAGE_CONTROLLERS;
            b.reload();
        }
        b.endGroup();
    }

    if (TgBot.getEnabled()) {
        for (auto *user : users) {
            if (b.beginGroup(String(F("Пользователь #")) + String(user->id))) {
                if (b.Switch(su::SH(String("ctrl_tg_en" + String(user->id)).c_str()), F("Включен"), &user->enabled)) {
                    b.reload();
                }
                if (user->enabled) {
                    b.Input(su::SH(String("ctrl_tg_name" + String(user->id)).c_str()), F("Имя"), &user->name);
                    b.Number(su::SH(String("ctrl_tg_cht" + String(user->id)).c_str()), F("ChatID"), &user->chatId);
                    b.Switch(su::SH(String("ctrl_tg_adm" + String(user->id)).c_str()), F("Админ"), &user->admin);
                    b.Switch(su::SH(String("ctrl_tg_ntf" + String(user->id)).c_str()), F("Уведомления"), &user->notify);
                }
                b.endGroup();
            }
        }
    }

    return curPage;
}

void TgbotPageClass::update(sets::Updater& upd)
{
    upd.update(WEB_GUI_TG_LAST_ID, TgBot.getLastID());  
}

/*********************************************************************/
/*                                                                   */
/*                          PRIVATE FUNCTIONS                        */
/*                                                                   */
/*********************************************************************/

TgbotPageClass TgbotPage;
