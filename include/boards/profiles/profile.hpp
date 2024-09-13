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

#ifndef __PROFILE_HPP__
#define __PROFILE_HPP__

typedef enum {
    PROF_MODE_INPUT,
    PROF_MODE_OUTPUT
} ProfGpioMode;

typedef enum {
    PROF_PULL_NONE,
    PROF_PULL_UP,
    PROF_PULL_DOWN
} ProfGpioPull;

typedef struct {
    unsigned id;
    unsigned addr;
} ProfExtender;

typedef struct {
    const char      *name;
    unsigned        pin;
    ProfGpioMode    mode;
    ProfGpioPull    pull;
    unsigned        extId;
} ProfGpio;

typedef struct {
    const char  *name;
    unsigned    rx;
    unsigned    tx;
    unsigned    rate;
} ProfUART;

typedef struct {
    const char  *name;
    unsigned    pin;
} ProfOneWire;

typedef struct {
    const char  *name;
    unsigned    sda;
    unsigned    scl;
} ProfI2C;

typedef struct {
    const char  *name;
    unsigned    miso;
    unsigned    mosi;
    unsigned    sck;
    unsigned    ss;
    unsigned    freq;
} ProfSPI;

typedef struct {
    const char      *name;
    unsigned        serial_rate;
    unsigned        mac_addr[6];
    ProfExtender    extenders[8];
    ProfGpio        gpio[32];
    ProfOneWire     onewire[4];
    ProfUART        uart[4];
    ProfI2C         i2c[2];
    ProfSPI         spi[4];
} BoardProfile;

#endif /* __PROFILE_HPP__ */
