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

#ifndef __SECURITY_PAGE_HPP__
#define __SECURITY_PAGE_HPP__

#include <SettingsAsync.h>
#include "pages.hpp"
#include "controllers/security.hpp"

class SecurityPageClass
{
public:
    WebGuiPage build(sets::Builder& b);
    void update(sets::Updater& upd);

private:
    size_t _getCurInput(const SecuritySensor *sensor, const std::vector<GpioPin *> &inputs) const;

};

extern SecurityPageClass SecurityPage; 

#endif /* __SECURITY_PAGE_HPP__ */
