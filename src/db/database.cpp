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

#include "db/database.hpp"

void Database::loadFromFile(const String &fileName)
{
    _fileName = fileName;

    if (Configs.getSource() == CFG_SRC_SD) {
        _file = SD.open("/" + fileName, "r");
    } else {
        _file = LittleFS.open("/" + fileName, "r");
    }

    if (_file.size() == 0) {
        _load = false;
    } else {
        _load = true;
        deserializeJson(_data, _file);
    }
}

void Database::saveToFile()
{
    if (Configs.getSource() == CFG_SRC_SD) {
        _file = SD.open("/" + _fileName, "w");
    } else {
        _file = LittleFS.open("/" + _fileName, "w");
    }
    serializeJsonPretty(_data, _file);
}

void Database::clear()
{
    _data.clear();
}

JsonDocument *Database::getData()
{
    return &_data;
}

void Database::close()
{
    _file.close();
}

bool Database::isLoad()
{
    return _load;
}
