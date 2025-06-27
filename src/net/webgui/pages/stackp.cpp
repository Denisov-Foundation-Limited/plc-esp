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

#include "net/webgui/pages/stackp.hpp"
#include "net/webgui/pages/elements.hpp"
#include "core/ifaces/ow.hpp"
#include "stack/stack.hpp"

/*********************************************************************/
/*                                                                   */
/*                          PUBLIC FUNCTIONS                         */
/*                                                                   */
/*********************************************************************/

WebGuiPage StackPage::build(sets::Builder& b)
{
    std::vector<StackUnit *>    units;
    WebGuiPage                  curPage = WEB_PAGE_STACK;

    Stack.getUnits(false, units);
    
    if (b.beginGroup(F("Стек"))) {
        bool enabled = Stack.getEnabled();
        if (b.Switch(F("Включен"), &enabled)) {
            Stack.setEnabled(b.build.value.toBool());
            b.reload();
        }
        if (b.Button(F("Назад"), sets::Colors::Aqua)) {
            curPage = WEB_PAGE_MAIN;
            b.reload();
        }
        b.endGroup();
    }

    if (Stack.getEnabled()) {
        for (auto *unit : units) {
            if (b.beginGroup(String(F("Юнит #")) + String(unit->id))) {
                if (b.Switch(su::SH(String("ctrl_stk_en" + String(unit->id)).c_str()), F("Включен"), &unit->enabled)) {
                    b.reload();
                }
                b.Input(su::SH(String("ctrl_stk_name" + String(unit->id)).c_str()), F("Имя"), &unit->name);
                b.Input(su::SH(String("ctrl_stk_ip" + String(unit->id)).c_str()), F("IP"), &unit->ip);
                b.LED(su::SH(String("ctrl_stk_onl" + String(unit->id)).c_str()), F("Онлайн"), unit->online);
                b.endGroup();
            }
        }
    }

    return curPage;
}

void StackPage::update(sets::Updater& upd)
{
    std::vector<StackUnit *>    units;

    Stack.getUnits(false, units);

    for (auto *unit : units) {
        if (unit->enabled) {
            upd.update(su::SH(String("ctrl_stk_onl" + String(unit->id)).c_str()), unit->online);
        }
    }    
}
