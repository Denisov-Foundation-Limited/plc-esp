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

#ifndef __CONTROLLERS_HPP__
#define __CONTROLLERS_HPP__

#include <Arduino.h>
#include <vector>

#include "ctrl.hpp"

class ControllersClass
{
public:
    void addController(Controller *ctrl);
    const std::vector<Controller *> &getControllers();
    Controller *getController(const String &name);
    void begin();
    void loop();
    bool isExists(const String &name) const;
    void remove(size_t idx);

private:
    std::vector<Controller *> _ctrls;
};

extern ControllersClass Controllers;

#endif /* __CONTROLLERS_HPP__ */