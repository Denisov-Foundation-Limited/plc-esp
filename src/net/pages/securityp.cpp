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

#include "net/pages/securityp.hpp"
#include "net/pages/elements.hpp"

/*********************************************************************/
/*                                                                   */
/*                          PUBLIC FUNCTIONS                         */
/*                                                                   */
/*********************************************************************/

WebGuiPage SecurityPageClass::build(sets::Builder& b)
{
    std::vector<SecuritySensor *>   sensors;
    WebGuiPage                      curPage = WEB_PAGE_SECURITY;
    std::vector<GpioPin *>          inputs;
    String                          sIns = "";

    SecurityCtrl.getSensors(false, sensors);
    
    if (b.beginGroup(F("Охрана"))) {
        if (b.Switch(F("Включен"), &SecurityCtrl.getEnabled())) {
            b.reload();
        }
        if (SecurityCtrl.getEnabled()) {
            if (b.Switch(F("Статус"), &SecurityCtrl.getStatus())) {
                SecurityCtrl.setStatus(b.build.value.toBool(), true);
            }
        }
        if (b.Button(F("Назад"), sets::Colors::Aqua)) {
            curPage = WEB_PAGE_CONTROLLERS;
            b.reload();
        }
        b.endGroup();
    }

    if (SecurityCtrl.getEnabled()) {
        Gpio.getPinsByType(GPIO_TYPE_INPUT, inputs);
        for (auto in : inputs) {
            sIns += in->name + ";";
        }

        for (auto *sensor : sensors) {
            if (b.beginGroup(String(F("Датчик #")) + String(sensor->id))) {
                if (b.Switch(su::SH(String("ctrl_sec_en" + String(sensor->id)).c_str()), F("Включен"), &sensor->enabled)) {
                    b.reload();
                }
                if (sensor->enabled) {
                    size_t  curInput = _getCurInput(sensor, inputs);
                    b.Input(su::SH(String("ctrl_sec_name" + String(sensor->id)).c_str()), F("Имя"), &sensor->name);
                    if (b.Select(su::SH(String("ctrl_sec_typ" + String(sensor->id)).c_str()), F("Тип"), F("Reed;PIR"), (uint8_t *)&sensor->type)) {
                        b.reload();
                    }
                    if (b.Select(su::SH(("ctrl_sec_in" + String(sensor->id)).c_str()), F("Вход"), sIns, &curInput)) {
                        if (b.build.value.toInt32() < inputs.size()) {
                            sensor->pin = inputs[b.build.value.toInt32()];
                            SecurityCtrl.begin(false);
                        } else {
                            Log.error(F("SECURITYP"), F("Incorrect GPIO input id"));
                        }
                    }
                    b.LED(su::SH(String("ctrl_sec_dtct" + String(sensor->id)).c_str()), F("Обнаружен"), &sensor->detected);
                }
                b.endGroup();
            }
        }
    }

    return curPage;
}

void SecurityPageClass::update(sets::Updater& upd)
{
    std::vector<SecuritySensor *>   sensors;

    SecurityCtrl.getSensors(false, sensors);

    for (auto *sensor : sensors) {
        upd.update(su::SH(String("ctrl_sec_en" + String(sensor->id)).c_str()), sensor->enabled);
        if (sensor->enabled) {
            upd.update(su::SH(String("ctrl_sec_name" + String(sensor->id)).c_str()), sensor->name);
            upd.update(su::SH(String("ctrl_sec_typ" + String(sensor->id)).c_str()), (uint8_t)sensor->type);
            upd.update(su::SH(String("ctrl_sec_dtct" + String(sensor->id)).c_str()), sensor->detected);
        }
    }    
}

/*********************************************************************/
/*                                                                   */
/*                          PRIVATE FUNCTIONS                        */
/*                                                                   */
/*********************************************************************/

size_t SecurityPageClass::_getCurInput(const SecuritySensor *sensor, const std::vector<GpioPin *> &inputs) const
{
    size_t  curInput = inputs.size();

    if (sensor->pin != nullptr) {
        for (size_t i = 0; i < inputs.size(); i++) {
            if (inputs[i]->id == sensor->pin->id) {
                curInput = i;
                break;
            }
        }
    }

    return curInput;
}

SecurityPageClass SecurityPage;
