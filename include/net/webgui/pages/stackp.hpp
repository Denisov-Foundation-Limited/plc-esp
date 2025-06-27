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

#ifndef __STACK_PAGE_HPP__
#define __STACK_PAGE_HPP__

#include <SettingsAsync.h>
#include "pages.hpp"

class StackPage
{
public:
    WebGuiPage build(sets::Builder& b);
    void update(sets::Updater& upd);
};

#endif /* __STACK_PAGE_HPP__ */
