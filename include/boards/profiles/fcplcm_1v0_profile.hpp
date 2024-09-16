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

#ifndef __FCPLCM_1V0_HPP__
#define __FCPLCM_1V0_HPP__

#include "profile.hpp"

BoardProfile PROGMEM ActiveBoard = {
    .name = "FCPLCM-1v0",
    .serial_rate = 115200,
    .mac_addr = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xFF },

    .extenders = {
    },

    .gpio = {
        { .name = "in-0/6",   .pin = 42, .mode = PROF_MODE_INPUT,  .pull = PROF_PULL_DOWN, .extId = 0 },
        { .name = "in-0/7",   .pin = 2,  .mode = PROF_MODE_INPUT,  .pull = PROF_PULL_DOWN, .extId = 0 },
        { .name = "in-0/8",   .pin = 46, .mode = PROF_MODE_INPUT,  .pull = PROF_PULL_DOWN, .extId = 0 },
        { .name = "rly-0/1",  .pin = 4,  .mode = PROF_MODE_OUTPUT, .pull = PROF_PULL_NONE, .extId = 0 },
        { .name = "rly-0/2",  .pin = 5,  .mode = PROF_MODE_OUTPUT, .pull = PROF_PULL_NONE, .extId = 0 },
        { .name = "rly-0/3",  .pin = 6,  .mode = PROF_MODE_OUTPUT, .pull = PROF_PULL_NONE, .extId = 0 },
        { .name = "rly-0/4",  .pin = 7,  .mode = PROF_MODE_OUTPUT, .pull = PROF_PULL_NONE, .extId = 0 },
        { .name = "rly-0/5",  .pin = 15, .mode = PROF_MODE_OUTPUT, .pull = PROF_PULL_NONE, .extId = 0 },
        { .name = "rly-0/6",  .pin = 16, .mode = PROF_MODE_OUTPUT, .pull = PROF_PULL_NONE, .extId = 0 },
        { .name = "buzzer",   .pin = 3,  .mode = PROF_MODE_OUTPUT, .pull = PROF_PULL_NONE, .extId = 0 },
        { .name = "led-net",  .pin = 38, .mode = PROF_MODE_OUTPUT, .pull = PROF_PULL_NONE, .extId = 0 },
        { .name = "rs485-io", .pin = 14, .mode = PROF_MODE_OUTPUT, .pull = PROF_PULL_NONE, .extId = 0 },
        { .name = "eth-irq",  .pin = 1,  .mode = PROF_MODE_OUTPUT, .pull = PROF_PULL_NONE, .extId = 0 },
    },

    .onewire = {
        { .name = "ow-sec",  .pin = 39 },
        { .name = "ow-temp", .pin = 40 }
    },

    .uart = {
        { .name = "uart-gsm",   .rx = 18, .tx = 17, .rate = 115200 },
        { .name = "uart-rs485", .rx = 47, .tx = 21, .rate = 9600   }
    },

    .i2c = {
        { .name = "i2c-1", .sda = 8, .scl = 9 }
    },

    .spi = {
        { .name = "spi-sd",  .miso = 12, .mosi = 11, .sck = 13, .ss = 41, .freq = 25 },
        { .name = "spi-eth", .miso = 12, .mosi = 11, .sck = 13, .ss = 10, .freq = 25 },
    },
};

#endif /* __FCPLCM_1V0_HPP__ */
