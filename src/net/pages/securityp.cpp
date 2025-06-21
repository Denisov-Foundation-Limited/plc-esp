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
#include "core/ifaces/ow.hpp"

/*********************************************************************/
/*                                                                   */
/*                          PUBLIC FUNCTIONS                         */
/*                                                                   */
/*********************************************************************/

WebGuiPage SecurityPageClass::build(sets::Builder& b)
{
    std::vector<SecuritySensor *>   sensors;
    std::vector<SecurityKey *>      keys;
    WebGuiPage                      curPage = WEB_PAGE_SECURITY;
    std::vector<GpioPin *>          inputs;
    String                          sIns = "";
    std::vector<uint64_t>           keySerials;

    SecurityCtrl.getSensors(false, sensors);
    SecurityCtrl.getKeys(false, keys);
    
    if (b.beginGroup(F("Охрана"))) {
        bool enabled = SecurityCtrl.getEnabled();
        if (b.Switch(F("Включен"), &enabled)) {
            SecurityCtrl.setEnabled(b.build.value.toBool());
            b.reload();
        }
        if (SecurityCtrl.getEnabled()) {
            bool status = SecurityCtrl.getStatus();
            bool alarm = SecurityCtrl.getAlarm();
            if (b.Switch(WEB_GUI_CTRL_SECURITY_STATUS, F("Статус"), &status)) {
                SecurityCtrl.setStatus(b.build.value.toBool(), true);
            }
            b.LED(WEB_GUI_CTRL_SECURITY_ALARM, F("Сирена"), &alarm);

            if (b.beginMenu(F("Ключи"))) {
                String sKeys = "";

                SecurityCtrl.readKeysFromBus(keySerials);

                for (auto k : keySerials) {
                    sKeys += String(k, 16) + ";";
                }

                for (auto *key : keys) {
                    if (b.beginGroup(String(F("Ключ #")) + String(key->id))) {
                        if (b.Switch(su::SH(String("ctrl_sec_ken" + String(key->id)).c_str()), F("Включен"), &key->enabled)) {
                            b.reload();
                        }
                        if (key->enabled) {
                            size_t curKey = _getCurKey(key, keySerials);
                            String localKeys;

                            if (keySerials.size() == 0 && key->serial != 0) {
                                localKeys = String(key->serial, 16);
                            } else {
                                localKeys = sKeys;
                                curKey = 0;
                            }
                            localKeys.toUpperCase();

                            b.Input(su::SH(String("ctrl_sec_kname" + String(key->id)).c_str()), F("Имя"), &key->name);
                            if (b.Select(su::SH(String("ctrl_sec_kserial" + String(key->id)).c_str()), F("Серийный номер"), localKeys, &curKey)) {
                                if (b.build.value.toInt32() < keySerials.size()) {
                                    key->serial = keySerials[b.build.value.toInt32()];
                                } else {
                                    Log.error(F("SECURITYP"), F("Incorrect key serial id"));
                                }
                            }
                        }
                        b.endGroup();
                    }
                }
                b.endMenu();
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

    upd.update(WEB_GUI_CTRL_SECURITY_STATUS, SecurityCtrl.getStatus());
    upd.update(WEB_GUI_CTRL_SECURITY_ALARM, SecurityCtrl.getAlarm());

    for (auto *sensor : sensors) {
        if (sensor->enabled) {
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

size_t SecurityPageClass::_getCurKey(const SecurityKey *key, const std::vector<uint64_t> &serials) const
{
    size_t  curKey = serials.size();

    if (key->serial != 0) {
        for (size_t i = 0; i < serials.size(); i++) {
            if (serials[i]== key->serial) {
                curKey = i;
                break;
            }
        }
    }

    return curKey;
}

SecurityPageClass SecurityPage;
