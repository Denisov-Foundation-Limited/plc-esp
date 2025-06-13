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

#ifndef __TGBOT_PAGE_HPP__
#define __TGBOT_PAGE_HPP__

#include <SettingsAsync.h>
#include "pages.hpp"
#include "controllers/meteo.hpp"

class TgbotPageClass
{
public:
    WebGuiPage build(sets::Builder& b);
    void update(sets::Updater& upd);

private:

};

extern TgbotPageClass TgbotPage; 

#endif /* __TGBOT_PAGE_HPP__ */

