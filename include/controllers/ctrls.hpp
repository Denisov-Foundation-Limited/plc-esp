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

#ifndef __CONTROLLERS_HPP__
#define __CONTROLLERS_HPP__

#include <Arduino.h>
#include <vector>

#include "ctrl.hpp"

class ControllersClass
{
public:
    void begin();
    void loop();
};

extern ControllersClass Controllers;

#endif /* __CONTROLLERS_HPP__ */