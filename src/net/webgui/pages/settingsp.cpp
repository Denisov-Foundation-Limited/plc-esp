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

#include "net/webgui/pages/settingsp.hpp"
#include "net/webgui/pages/elements.hpp"
#include "core/clock.hpp"
#include "utils/configs.hpp"

/*********************************************************************/
/*                                                                   */
/*                          PUBLIC FUNCTIONS                         */
/*                                                                   */
/*********************************************************************/

WebGuiPage SettingsPage::build(sets::Builder& b)
{
    WebGuiPage  curPage = WEB_PAGE_SETTINGS;

    if (b.beginGroup(F("Часы"))) {
        Datime  time;
        Clock.getTime(time);

        uint32_t t = time.getUnix();
        if (b.DateTime(WEB_GUI_SYS_DTIME, F("Дата и время"), &t)) {
            Datime  t(b.build.value.toInt32());
            Clock.setTimeUTC(t);
        }
        uint8_t utc = Clock.getUTC();
        if (b.Slider(WEB_GUI_SYS_UTC_SET, "UTC", CLOCK_UTC_MIN, CLOCK_UTC_MAX, 1, "h", &utc)) {
            Clock.setUTC(utc);
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
        if (b.Button(F("Сохранить"))) {
            Configs.writeAll();
            curPage = WEB_PAGE_MAIN;
            b.reload();
        }
        if (b.Button(F("Удалить"), sets::Colors::Red)) {
            Configs.eraseAll();
            curPage = WEB_PAGE_MAIN;
            b.reload();
        }
        b.endButtons();
        b.endGroup();
    }

    if (b.beginGroup(F("Система"))) {
        b.beginButtons();
        if (b.Button(F("Рестарт"))) {
            ESP.restart();
        }
        b.endButtons();
        b.endGroup();
    }
    
    return curPage;
}

void SettingsPage::update(sets::Updater& upd)
{
    Datime  time;
    Clock.getTime(time);
    uint32_t t = time.getUnix();

    upd.update(WEB_GUI_SYS_DTIME, t);      
    upd.update(WEB_GUI_SYS_UTC_SET, Clock.getUTC());
    upd.update(WEB_GUI_SYS_TEMP, Plc.getBoardTemp());
    upd.update(WEB_GUI_SYS_FAN_STATUS, Plc.getFanStatus());
}
