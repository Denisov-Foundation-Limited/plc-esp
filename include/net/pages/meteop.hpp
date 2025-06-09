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

#ifndef __METEO_PAGE_HPP__
#define __METEO_PAGE_HPP__

#include <SettingsAsync.h>
#include "pages.hpp"
#include "controllers/meteo.hpp"

class MeteoPageClass
{
public:
    WebGuiPage build(sets::Builder& b);
    void update(sets::Updater& upd);

private:
    size_t _getCurAddr(MeteoSensor *sensor, const std::vector<uint64_t> &owSens) const;
};

extern MeteoPageClass MeteoPage; 

#endif /* __METEO_PAGE_HPP__ */

