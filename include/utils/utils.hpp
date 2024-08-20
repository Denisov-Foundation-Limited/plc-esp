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

#ifndef __UTILS_HPP__
#define __UTILS_HPP__

#include <Arduino.h>

bool splitString(String str, String delim, std::vector<String> &result);

#endif /* __UTILS_HPP__ */
