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

#define EE_DB_ID_MAX        64
#define EE_DB_MAGIC         0x4A5C
#define EE_DB_FLASH_BLOCK   1024
#define EE_DB_BLOCK_COUNT   512

typedef enum {
    EE_DB_OFFSET_MAGIC,
    EE_DB_OFFSET_SOCKET,
    EE_DB_OFFSET_CLIMATE,
    EE_DB_OFFSET_SECURITY,
    EE_DB_OFFSET_MAX
} EeDbOffset;

typedef struct {
    uint64_t status;
} EeDbSocket;

typedef struct {
    int8_t  temp;
    uint8_t delta;
} EeDbClimateTemp;

typedef struct {
    uint64_t        status;
    EeDbClimateTemp tempCfg[EE_DB_ID_MAX];
} EeDbClimate;

typedef struct {
    bool status;
} EeDbSecurity;

class EepromDbClass
{
public:
    EepromDbClass();
    bool begin();
    bool &getEnabled();
    size_t getOffset(EeDbOffset offset);

    bool loadSocketDb(EeDbSocket &data);
    bool saveSocketDb(EeDbSocket &data);
    bool getSocketStatus(EeDbSocket &data, uint8_t id, bool &status);
    bool setSocketStatus(EeDbSocket &data, uint8_t id, bool status);

    bool loadClimateDb(EeDbClimate &data);
    bool saveClimateDb(EeDbClimate &data);
    bool getClimateData(EeDbClimate &data, uint8_t id, bool &status, int8_t &temp, uint8_t &delta);
    bool setClimateData(EeDbClimate &data, uint8_t id, bool status, int8_t temp, uint8_t delta);

    bool loadSecurityDb(EeDbSecurity &data);
    bool saveSecurityDb(EeDbSecurity &data);
    bool getSecurityStatus(EeDbSecurity &data, bool &status);
    bool setSecurityStatus(EeDbSecurity &data, bool status);

private:
    bool        _enabled = true;
    I2C_eeprom  _ee;
};

extern EepromDbClass EeDb;

#endif /* __EEPROM_DB_HPP__ */
