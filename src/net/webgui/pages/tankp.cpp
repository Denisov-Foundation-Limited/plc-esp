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

#include "net/webgui/pages/tankp.hpp"
#include "net/webgui/pages/elements.hpp"

/*********************************************************************/
/*                                                                   */
/*                          PUBLIC FUNCTIONS                         */
/*                                                                   */
/*********************************************************************/

WebGuiPage TankPage::build(sets::Builder& b)
{
    std::vector<Tank *>     tanks;
    std::vector<GpioPin *>  relays, inputs;
    String                  sSensors = "", sIns = "", sRlys = "";
    WebGuiPage              curPage = WEB_PAGE_TANK;

    TankCtrl.getTanks(false, tanks);
    
    if (b.beginGroup(F("Баки"))) {
        bool enabled = TankCtrl.getEnabled();
        if (b.Switch(F("Включен"), &enabled)) {
            TankCtrl.setEnabled(b.build.value.toBool());
            b.reload();
        }
        if (b.Button(F("Назад"), sets::Colors::Aqua)) {
            curPage = WEB_PAGE_CONTROLLERS;
            b.reload();
        }
        b.endGroup();
    }

    if (TankCtrl.getEnabled()) {
        Gpio.getPinsByType(GPIO_TYPE_RELAY, relays);
        Gpio.getPinsByType(GPIO_TYPE_INPUT, inputs);
        for (auto in : inputs) {
            sIns += in->name + ";";
        }
        for (auto rly : relays) {
            sRlys += rly->name + ";";
        }
        sRlys += ";";

        for (auto *tank : tanks) {
            if (b.beginGroup(String(F("Бак #")) + String(tank->id))) {
                if (b.Switch(su::SH(String("ctrl_tank_en" + String(tank->id)).c_str()), F("Включен"), &tank->enabled)) {
                    b.reload();
                }
                if (tank->enabled) {
                    size_t  curPump = _getCurPump(tank, relays);
                    size_t  curValve = _getCurValve(tank, relays);
                    size_t  curLvlLow = _getCurLevel(tank, TANK_LEVEL_LOW, inputs);
                    size_t  curLvlMid = _getCurLevel(tank, TANK_LEVEL_MIDDLE, inputs);
                    size_t  curLvlHgh = _getCurLevel(tank, TANK_LEVEL_HIGH, inputs);

                    b.Input(su::SH(String("ctrl_tank_name" + String(tank->id)).c_str()), F("Имя"), &tank->name);
                    bool status = tank->status;
                    if (b.Switch(su::SH(String("ctrl_tank_sts_" + String(tank->id)).c_str()), F("Статус"), &status)) {
                        TankCtrl.setStatus(tank, b.build.value.toBool(), true);
                    }
                    if (b.Select(su::SH(("ctrl_tank_pump" + String(tank->id)).c_str()), F("Насос"), sRlys, &curPump)) {
                        if (b.build.value.toInt32() < relays.size()) {
                            tank->pump = relays[b.build.value.toInt32()];
                            TankCtrl.begin(false);
                        } else {
                            Log.error(F("TANKP"), F("Incorrect GPIO pump id"));
                        }
                    }
                    if (b.Select(su::SH(("ctrl_tank_vlv" + String(tank->id)).c_str()), F("Кран"), sRlys, &curValve)) {
                        if (b.build.value.toInt32() < relays.size()) {
                            tank->valve = relays[b.build.value.toInt32()];
                            TankCtrl.begin(false);
                        } else {
                            Log.error(F("TANKP"), F("Incorrect GPIO valve id"));
                        }
                    }
                    if (b.Select(su::SH(("ctrl_tank_low" + String(tank->id)).c_str()), F("Уровень Низкий"), sIns, &curLvlLow)) {
                        if (b.build.value.toInt32() < inputs.size()) {
                            tank->levels[TANK_LEVEL_LOW] = inputs[b.build.value.toInt32()];
                            TankCtrl.begin(false);
                        } else {
                            Log.error(F("TANKP"), F("Incorrect GPIO level low id"));
                        }
                    }
                    if (b.Select(su::SH(("ctrl_tank_mid" + String(tank->id)).c_str()), F("Уровень Средний"), sIns, &curLvlMid)) {
                        if (b.build.value.toInt32() < inputs.size()) {
                            tank->levels[TANK_LEVEL_MIDDLE] = inputs[b.build.value.toInt32()];
                            TankCtrl.begin(false);
                        } else {
                            Log.error(F("TANKP"), F("Incorrect GPIO level mid id"));
                        }
                    }
                    if (b.Select(su::SH(("ctrl_tank_hgh" + String(tank->id)).c_str()), F("Уровень Высокий"), sIns, &curLvlHgh)) {
                        if (b.build.value.toInt32() < inputs.size()) {
                            tank->levels[TANK_LEVEL_HIGH] = inputs[b.build.value.toInt32()];
                            TankCtrl.begin(false);
                        } else {
                            Log.error(F("TANKP"), F("Incorrect GPIO level high id"));
                        }
                    }
                    b.LED(su::SH(("ctrl_tank_vlvl" + String(tank->id)).c_str()), F("Набор воды"), (tank->valve != nullptr) ? tank->valve->state : false);
                    b.LED(su::SH(("ctrl_tank_pmpl" + String(tank->id)).c_str()), F("Насос"), (tank->pump != nullptr) ? tank->pump->state : false);
                    String sLevel = String(tank->level) + "%";
                    b.Label(su::SH(("ctrl_tank_lvls" + String(tank->id)).c_str()), F("Уровень"), sLevel);
                }
                b.endGroup();
            }
        }
    }

    return curPage;
}

void TankPage::update(sets::Updater& upd)
{
    std::vector<Tank *>    tanks;

    TankCtrl.getTanks(false, tanks);

    for (auto *tank : tanks) {
        if (tank->enabled) {
            upd.update(su::SH(String("ctrl_tank_sts_" + String(tank->id)).c_str()), tank->status);
            upd.update(su::SH(String("ctrl_tank_vlvl" + String(tank->id)).c_str()), (tank->valve != nullptr) ? tank->valve->state : false);
            upd.update(su::SH(String("ctrl_tank_pmpl" + String(tank->id)).c_str()), (tank->pump != nullptr) ? tank->pump->state : false);
            String sLevel = String(tank->level) + "%";
            upd.update(su::SH(String("ctrl_tank_lvls" + String(tank->id)).c_str()), sLevel);
        }
    }    
}

/*********************************************************************/
/*                                                                   */
/*                          PRIVATE FUNCTIONS                        */
/*                                                                   */
/*********************************************************************/

size_t TankPage::_getCurPump(const Tank *tank, const std::vector<GpioPin *> &relays) const
{
    size_t  curRelay = relays.size();

    if (tank->pump != nullptr) {
        for (size_t i = 0; i < relays.size(); i++) {
            if (relays[i]->id == tank->pump->id) {
                curRelay = i;
                break;
            }
        }
    }

    return curRelay;
}

size_t TankPage::_getCurValve(const Tank *tank, const std::vector<GpioPin *> &relays) const
{
    size_t  curRelay = relays.size();

    if (tank->valve != nullptr) {
        for (size_t i = 0; i < relays.size(); i++) {
            if (relays[i]->id == tank->valve->id) {
                curRelay = i;
                break;
            }
        }
    }

    return curRelay;
}

size_t TankPage::_getCurLevel(const Tank *tank, size_t level, const std::vector<GpioPin *> &inputs) const
{
    size_t  curInput = inputs.size();

    if (tank->levels[level] != nullptr) {
        for (size_t i = 0; i < inputs.size(); i++) {
            if (inputs[i]->id == tank->levels[level]->id) {
                curInput = i;
                break;
            }
        }
    }

    return curInput;
}
