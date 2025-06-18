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

#include "net/pages/climatep.hpp"
#include "net/pages/elements.hpp"

/*********************************************************************/
/*                                                                   */
/*                          PUBLIC FUNCTIONS                         */
/*                                                                   */
/*********************************************************************/

WebGuiPage ClimatePageClass::build(sets::Builder& b)
{
    std::vector<ClimateZone *>  zones;
    std::vector<MeteoSensor *>  sensors;
    std::vector<GpioPin *>      relays, buttons;
    String                      sSensors = "", sBtns = "", sRlys = "";
    WebGuiPage                  curPage = WEB_PAGE_CLIMATE;

    ClimateCtrl.getZones(false, zones);

    if (b.beginGroup(F("Климат"))) {
        if (b.Switch(WEB_GUI_CTRL_CLIMATE_ENABLE, F("Включен"), &ClimateCtrl.getEnabled())) {
            b.reload();
        }
        if (b.Button(F("Назад"), sets::Colors::Aqua)) {
            curPage = WEB_PAGE_CONTROLLERS;
            b.reload();
        }
        b.endGroup();
    }

    if (ClimateCtrl.getEnabled()) {
        MeteoCtrl.getSensors(true, sensors);
        Gpio.getPinsByType(GPIO_TYPE_RELAY, relays);
        Gpio.getPinsByType(GPIO_TYPE_INPUT, buttons);

        for (auto btn : buttons) {
            sBtns += btn->name + ";";
        }
        for (auto rly : relays) {
            sRlys += rly->name + ";";
        }
        for (auto sensor : sensors) {
            sSensors += sensor->name + ";";
        }

        for (auto *zone : zones) {
            if (b.beginGroup(String(F("Зона #")) + String(zone->id))) {
                if (b.Switch(su::SH(String("ctrl_clmt_en_" + String(zone->id)).c_str()), F("Включить"), &zone->enabled)) {
                    b.reload();
                }
                if (zone->enabled) {
                    size_t  curSensor = _getCurSensor(zone, sensors);
                    size_t  curRelay = _getCurRelay(zone, relays);
                    size_t  curButton = _getCurButton(zone, buttons);

                    if (b.Switch(su::SH(String("ctrl_clmt_sts_" + String(zone->id)).c_str()), F("Статус"), &ClimateCtrl.getStatus(zone))) {
                        ClimateCtrl.setStatus(zone, b.build.value.toBool(), true);
                    }
                    b.Input(su::SH(String("ctrl_clmt_name_" + String(zone->id)).c_str()), F("Имя"), &zone->name);
                    if (b.Slider(su::SH(("ctrl_clmt_temp_" + String(zone->id)).c_str()), "Порог", -20, 40, 1, F("°"), &zone->temp)) {
                        ClimateCtrl.setTemp(zone, b.build.value.toInt(), true);
                    }
                    if (b.Slider(su::SH(("ctrl_clmt_dlt_" + String(zone->id)).c_str()), "Дельта", 0, 10, 1, F("°"), &zone->delta)) {
                        ClimateCtrl.setDelta(zone, b.build.value.toInt(), true);
                    }
                    if (b.Select(su::SH(("ctrl_clmt_mod_" + String(zone->id)).c_str()), F("Режим"), F("Обогрев;Охлаждение"), (uint8_t *)&zone->type)) {
                        zone->work = false;
                    }
                    if (b.Select(su::SH(("ctrl_clmt_s" + String(zone->id)).c_str()), F("Датчик"), sSensors, &curSensor)) {
                        if (b.build.value.toInt32() < sensors.size()) {
                            zone->sensor = sensors[b.build.value.toInt32()];
                        } else {
                            Log.error(F("CLIMATEP"), F("Incorrect meteo sensor id"));
                        }
                    }
                    if (b.Select(su::SH(("ctrl_clmt_rly" + String(zone->id)).c_str()), F("Реле"), sRlys, &curRelay)) {
                        if (b.build.value.toInt32() < relays.size()) {
                            zone->relay = relays[b.build.value.toInt32()];
                            ClimateCtrl.begin(false);
                        } else {
                            Log.error(F("CLIMATEP"), F("Incorrect GPIO relay id"));
                        }
                    }
                    if (b.Select(su::SH(("ctrl_clmt_btn" + String(zone->id)).c_str()), F("Кнопка"), sBtns, &curButton)) {
                        if (b.build.value.toInt32() < buttons.size()) {
                            zone->button = buttons[b.build.value.toInt32()];
                            ClimateCtrl.begin(false);
                        } else {
                            Log.error(F("CLIMATEP"), F("Incorrect GPIO button id"));
                        }
                    }
                    b.Label(su::SH(("ctrl_clmt_sens_" + String(zone->id)).c_str()), F("Температура"), (zone->sensor != nullptr) ? (String(zone->sensor->data.temp) + "°") : String("N/A"));
                    b.LED(su::SH(("ctrl_clmt_work_" + String(zone->id)).c_str()), F("Активен"), &zone->work);
                }
                b.endGroup();
            }
        }
    }
    return curPage;
}

void ClimatePageClass::update(sets::Updater& upd)
{
    std::vector<ClimateZone *> zones;

    ClimateCtrl.getZones(false, zones);

    for (auto *zone : zones) {
        upd.update(su::SH(String("ctrl_clmt_en_" + String(zone->id)).c_str()), zone->enabled);
        if (zone->enabled) {
            upd.update(su::SH(String("ctrl_clmt_sts_" + String(zone->id)).c_str()), zone->status);
            upd.update(su::SH(String("ctrl_clmt_name_" + String(zone->id)).c_str()), zone->name);
            upd.update(su::SH(("ctrl_clmt_sens_" + String(zone->id)).c_str()), (zone->sensor != nullptr) ? (String(zone->sensor->data.temp) + "°") : String("N/A"));
            upd.update(su::SH(("ctrl_clmt_temp_" + String(zone->id)).c_str()), zone->temp);
            upd.update(su::SH(("ctrl_clmt_dlt_" + String(zone->id)).c_str()), zone->delta);
            upd.update(su::SH(("ctrl_clmt_mod_" + String(zone->id)).c_str()), (uint8_t)zone->type);
            upd.update(su::SH(("ctrl_clmt_work_" + String(zone->id)).c_str()), zone->work);
        }
    }
}

/*********************************************************************/
/*                                                                   */
/*                          PRIVATE FUNCTIONS                        */
/*                                                                   */
/*********************************************************************/

size_t ClimatePageClass::_getCurSensor(const ClimateZone *zone, const std::vector<MeteoSensor *> &sensors) const
{
    size_t  curSensor = sensors.size();

    if (zone->sensor != nullptr) {
        for (size_t i = 0; i < sensors.size(); i++) {
            if (sensors[i]->name == zone->sensor->name) {
                curSensor = i;
                break;
            }
        }
    }

    return curSensor;
}

size_t ClimatePageClass::_getCurRelay(const ClimateZone *zone, const std::vector<GpioPin *> &relays) const
{
    size_t  curRelay = relays.size();

    if (zone->relay != nullptr) {
        for (size_t i = 0; i < relays.size(); i++) {
            if (relays[i]->id == zone->relay->id) {
                curRelay = i;
                break;
            }
        }
    }

    return curRelay;
}

size_t ClimatePageClass::_getCurButton(const ClimateZone *zone, const std::vector<GpioPin *> &buttons) const
{
    size_t  curButton = buttons.size();

    if (zone->button != nullptr) {
        for (size_t i = 0; i < buttons.size(); i++) {
            if (buttons[i]->id == zone->button->id) {
                curButton = i;
                break;
            }
        }
    }

    return curButton;
}

ClimatePageClass ClimatePage;
