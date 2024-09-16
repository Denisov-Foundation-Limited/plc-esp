/**********************************************************************/
/*                                                                    */
/* Programmable Logic Controller for ESP microcontrollers             */
/*                                                                    */
/* Copyright (C) 2024 Denisov Foundation Limited                      */
/* License: GPLv3                                                     */
/* Written by Sergey Denisov aka LittleBuster                         */
/* Email: DenisovFoundationLtd@gmail.com                              */
/*                                                                    */
/**********************************************************************/

#include "controllers/ctrls.hpp"

/*********************************************************************/
/*                                                                   */
/*                          PUBLIC FUNCTIONS                         */
/*                                                                   */
/*********************************************************************/

void ControllersClass::addController(Controller *ctrl)
{
    _ctrls.push_back(ctrl);
}

bool ControllersClass::isExists(const String &name) const
{
    for (auto c : _ctrls) {
        if (c->getName() == name) {
            return true;
        }
    }
    return false;
}

const std::vector<Controller *> &ControllersClass::getControllers()
{
    return _ctrls;
}

Controller *ControllersClass::getController(const String &name)
{
    for (auto c : _ctrls) {
        if (c->getName() == name) {
            return c;
        }
    }
    return nullptr;
}

void ControllersClass::begin()
{
    for (auto c : _ctrls) {
        c->begin();
    }
}

void ControllersClass::loop()
{
    for (auto c : _ctrls) {
        c->loop();
    }
}

ControllersClass Controllers;
