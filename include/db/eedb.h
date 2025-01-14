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

#ifndef __EEPROM_DB_HPP__
#define __EEPROM_DB_HPP__

#include <Arduino.h>
#include "I2C_eeprom.h"

#define RR_DB_ID_MAX    64

typedef enum {
    EE_DB_OFFSET_SOCKET
} EeDbOffset;

typedef struct {
    uint64_t status;
} EeDbSocket;

class EepromDbClass
{
public:
    EepromDbClass();
    bool begin();
    bool getEnabled() const;
    size_t getOffset(EeDbOffset offset);

    bool loadSocketDb(EeDbSocket &sockdb);
    bool saveSocketDb(EeDbSocket &sockdb);
    bool getSocketStatus(EeDbSocket &sockdb, uint8_t id, bool &status);
    bool setSocketStatus(EeDbSocket &sockdb, uint8_t id, bool status);

private:
    bool _enabled = true;
    I2C_eeprom _ee;
};

extern EepromDbClass EeDb;

#endif /* __EEPROM_DB_HPP__ */