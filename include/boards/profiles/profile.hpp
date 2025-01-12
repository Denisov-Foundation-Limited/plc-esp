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

#ifndef __PROFILE_HPP__
#define __PROFILE_HPP__

#include <Arduino.h>

#define PROF_GPIO_MAX   40
#define PROF_SPI_MAX    2
#define PROF_I2C_MAX    2
#define PROF_EXT_MAX    10
#define PROF_OW_MAX     2
#define PROF_UART_MAX   1
#define PROF_RELAYS_MAX 8

typedef enum {
    PROF_GPIO_SENSOR,
    PROF_GPIO_RELAY,
    PROF_GPIO_INPUT,
    PROF_GPIO_GENERIC,
    PROF_GPIO_BUZZER
} ProfGpioType;

typedef struct {
    uint8_t id;
    uint8_t i2c;
    uint8_t addr;
} ProfExt;

typedef struct {
    uint16_t        id;
    uint8_t         pin;
    ProfGpioType    type;
    uint8_t         ext;
} ProfGpio;

typedef struct {
    uint8_t id;
    uint8_t miso;
    uint8_t mosi;
    uint8_t sck;
    uint8_t ss;
} ProfSPI;

typedef struct {
    uint8_t id;
    uint8_t pin;
} ProfOneWire;

typedef struct {
    uint8_t id;
    uint8_t sda;
    uint8_t scl;
} ProfI2C;

typedef struct {
    uint8_t     id;
    uint8_t     rx;
    uint8_t     tx;
    unsigned    speed;
} ProfUART;

typedef struct {
    bool        enabled;
    const char *ssid;
    const char *passwd;
    const char *hostname;
    struct {
        uint16_t    net;
    } gpio;
} ProfWiFi;

typedef struct {
    ProfI2C     i2c[PROF_OW_MAX];
    ProfExt     ext[PROF_EXT_MAX];
    ProfGpio    gpio[PROF_GPIO_MAX];
    ProfSPI     spi[PROF_SPI_MAX];
    ProfOneWire ow[PROF_I2C_MAX];
    ProfUART    uart[PROF_UART_MAX];
} ProfIf;

typedef struct {
    uint16_t    fan;
    uint16_t    status;
    uint16_t    alarm;
    uint16_t    relays[PROF_RELAYS_MAX];
    uint16_t    up;
    uint16_t    middle;
    uint16_t    down;
} ProfPlcGpio;

typedef struct {
    uint16_t    i2c;
    uint16_t    addr;
} ProfPlcTemp;

typedef struct {
    ProfPlcGpio gpio;
    ProfPlcTemp temp;
    bool        fan;
} ProfPlc;

typedef struct {
    const char  *name;
    unsigned    serial_rate;
    ProfIf      interfaces;
    ProfPlc     plc;
    ProfWiFi    wifi;
} BoardProfile;

#endif /* __PROFILE_HPP__ */
