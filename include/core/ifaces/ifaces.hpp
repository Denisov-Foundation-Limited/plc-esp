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

#ifndef __INTERFACES_HPP__
#define __INTERFACES_HPP__

#include <Arduino.h>
#include <vector>

#include "core/ifaces/iface.hpp"
#include "utils/log.hpp"

class InterfacesClass
{
public:
    void addInterface(Interface *iface);
    Interface *getInterface(const String &name);
    const std::vector<Interface *> &getInterfaces() const;

private:
    std::vector<Interface *>  _ifaces;
};

extern InterfacesClass Interfaces;

#endif /* __INTERFACES_HPP__ */
