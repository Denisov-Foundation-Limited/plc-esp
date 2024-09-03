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

#include "utils/utils.hpp"

bool UtilsClass::splitString(String str, String delim, std::vector<String> &result)
{
    if (str.indexOf(delim) == -1) {
        return false;
    }

    do {
        String subStr = str.substring(0, str.indexOf(delim));
        result.push_back(subStr);
        str.remove(0, subStr.length() + 1);
    } while ((str.indexOf(delim) > -1));

    if (str != "") {
        result.push_back(str);
    }

    return true;
}

UtilsClass Utils;
