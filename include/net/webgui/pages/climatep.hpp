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

#ifndef __CLIMATE_PAGE_HPP__
#define __CLIMATE_PAGE_HPP__

#include <SettingsAsync.h>
#include "pages.hpp"
#include "controllers/climate.hpp"

class ClimatePage
{
public:
    WebGuiPage build(sets::Builder& b);
    void update(sets::Updater& upd);

private:
    size_t _getCurSensor(const ClimateZone *zone, const std::vector<MeteoSensor *> &sensors) const;
    size_t _getCurRelay(const ClimateZone *zone, const std::vector<GpioPin *> &relays) const;
    size_t _getCurButton(const ClimateZone *zone, const std::vector<GpioPin *> &buttons) const;
};

#endif /* __CLIMATE_PAGE_HPP__ */

