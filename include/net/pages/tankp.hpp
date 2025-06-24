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

#ifndef __TANK_PAGE_HPP__
#define __TANK_PAGE_HPP__

#include <SettingsAsync.h>
#include "pages.hpp"
#include "controllers/tank.hpp"

class TankPageClass
{
public:
    WebGuiPage build(sets::Builder& b);
    void update(sets::Updater& upd);

private:
    size_t _getCurPump(const Tank *tank, const std::vector<GpioPin *> &relays) const;
    size_t _getCurValve(const Tank *tank, const std::vector<GpioPin *> &relays) const;
    size_t _getCurLevel(const Tank *tank, size_t level, const std::vector<GpioPin *> &inputs) const;
};

extern TankPageClass TankPage; 

#endif /* __TANK_PAGE_HPP__ */

