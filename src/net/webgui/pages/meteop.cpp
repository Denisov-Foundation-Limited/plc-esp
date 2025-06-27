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

#include "net/webgui/pages/meteop.hpp"
#include "net/webgui/pages/elements.hpp"

/*********************************************************************/
/*                                                                   */
/*                          PUBLIC FUNCTIONS                         */
/*                                                                   */
/*********************************************************************/

WebGuiPage MeteoPage::build(sets::Builder& b)
{
    std::vector<MeteoSensor *>  sensors;
    WebGuiPage                  curPage = WEB_PAGE_METEO;

    MeteoCtrl.getSensors(false, sensors);
    
    if (b.beginGroup(F("Метео"))) {
        bool enabled = MeteoCtrl.getEnabled();
        if (b.Switch(WEB_GUI_CTRL_METEO_ENABLE, F("Включен"), &enabled)) {
            MeteoCtrl.setEnabled(b.build.value.toBool());
            b.reload();
        }
        if (b.Button(F("Назад"), sets::Colors::Aqua)) {
            curPage = WEB_PAGE_CONTROLLERS;
            b.reload();
        }
        b.endGroup();
    }

    if (MeteoCtrl.getEnabled()) {
        std::vector<uint64_t> owSens;
        MeteoCtrl.findDsSensors(owSens);
        String sOwSens = "";

        for (auto s : owSens) {
            sOwSens += String(s, 16) + ";";
        }

        for (auto *sensor : sensors) {
            if (b.beginGroup(String(F("Датчик #")) + String(sensor->id))) {
                if (b.Switch(su::SH(String("ctrl_meteo_en" + String(sensor->id)).c_str()), F("Включен"), &sensor->enabled)) {
                    b.reload();
                }
                if (sensor->enabled) {
                    b.Input(su::SH(String("ctrl_meteo_name" + String(sensor->id)).c_str()), F("Имя"), &sensor->name);
                    if (b.Select(su::SH(String("ctrl_meteo_typ" + String(sensor->id)).c_str()), F("Тип"), F("AM2302;DS18B20;BME280;DHT22"), (uint8_t *)&sensor->type)) {
                        b.reload();
                    }
                    if (sensor->type == METEO_SENSOR_DS18B20) {
                        size_t curAddr = _getCurAddr(sensor, owSens);
                        String localAddrs;

                        if (owSens.size() == 0 && sensor->addr != 0) {
                            localAddrs = String(sensor->addr, 16);
                        } else {
                            localAddrs = sOwSens;
                            curAddr = 0;
                        }
                        localAddrs.toUpperCase();
            
                        if (b.Select(su::SH(String("ctrl_meteo_ds_addr" + String(sensor->id)).c_str()), F("Адрес"), localAddrs, &curAddr)) {
                            if (b.build.value.toInt32() < owSens.size()) {
                                sensor->addr = owSens[b.build.value.toInt32()];
                            } else {
                                Log.error(F("METEOP"), F("Incorrect OW sensor id"));
                            }
                        }
                        b.Label(su::SH(("ctrl_meteo_temp" + String(sensor->id)).c_str()), F("Температура"), String(sensor->data.temp) + "°");
                    }
                }
                b.endGroup();
            }
        }
    }

    return curPage;
}

void MeteoPage::update(sets::Updater& upd)
{
    std::vector<MeteoSensor *>    sensors;

    MeteoCtrl.getSensors(false, sensors);

    for (auto *sensor : sensors) {
        if (sensor->enabled) {
            if (sensor->type == METEO_SENSOR_DS18B20) {
                upd.update(su::SH(String("ctrl_meteo_temp" + String(sensor->id)).c_str()), String(String(sensor->data.temp) + "°"));
            }
        }
    }    
}

/*********************************************************************/
/*                                                                   */
/*                          PRIVATE FUNCTIONS                        */
/*                                                                   */
/*********************************************************************/

size_t MeteoPage::_getCurAddr(MeteoSensor *sensor, const std::vector<uint64_t> &owSens) const
{
    size_t curAddr = owSens.size();

    for (size_t i = 0; i < owSens.size(); i++) {
        if (sensor->addr == owSens[i]) {
            curAddr = i;
            break;
        }
    }

    return curAddr;
}
