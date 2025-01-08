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

#ifndef __DATABASE_HPP__
#define __DATABASE_HPP__

#include <Arduino.h>
#include <ArduinoJson.h>
#include "utils/configs.hpp"
#include <LittleFS.h>
#include <SD.h>

class Database
{
public:
    void loadFromFile(const String &fileName);
    void saveToFile();
    JsonDocument *getData();
    void clear();
    void close();
    bool isLoad();

private:
    JsonDocument    _data;
    File            _file;
    bool            _load = false;
    String          _fileName = "";
};

#endif /* __DATABASE_HPP__ */
