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

#ifndef __FCPLCM_2V0_HPP__
#define __FCPLCM_2V0_HPP__

#include "profile.hpp"

BoardProfile PROGMEM ActiveBoard = {
    .name = "FCPLCM-2v0",
    .serial_rate = 115200,

    .extenders = {
    },

    .interfaces = {
        .gpio = {
            { .name = "rly-0/1", .type = PROF_TYPE_RELAY,  .pin = 16, .mode = PROF_MODE_OUTPUT, .pull = PROF_PULL_NONE, .extId = 0 },
            { .name = "rly-0/2", .type = PROF_TYPE_RELAY,  .pin = 6,  .mode = PROF_MODE_OUTPUT, .pull = PROF_PULL_NONE, .extId = 0 },
            { .name = "rly-0/3", .type = PROF_TYPE_RELAY,  .pin = 15, .mode = PROF_MODE_OUTPUT, .pull = PROF_PULL_NONE, .extId = 0 },
            { .name = "rly-0/4", .type = PROF_TYPE_RELAY,  .pin = 7,  .mode = PROF_MODE_OUTPUT, .pull = PROF_PULL_NONE, .extId = 0 },
            { .name = "rly-0/5", .type = PROF_TYPE_RELAY,  .pin = 5,  .mode = PROF_MODE_OUTPUT, .pull = PROF_PULL_NONE, .extId = 0 },
            { .name = "rly-0/6", .type = PROF_TYPE_RELAY,  .pin = 4,  .mode = PROF_MODE_OUTPUT, .pull = PROF_PULL_NONE, .extId = 0 },
            { .name = "rly-0/7", .type = PROF_TYPE_RELAY,  .pin = 42, .mode = PROF_MODE_OUTPUT, .pull = PROF_PULL_NONE, .extId = 0 },
            { .name = "rly-0/8", .type = PROF_TYPE_RELAY,  .pin = 41, .mode = PROF_MODE_OUTPUT, .pull = PROF_PULL_NONE, .extId = 0 },
            { .name = "in-0/1",  .type = PROF_TYPE_DINPUT, .pin = 2,  .mode = PROF_MODE_INPUT,  .pull = PROF_PULL_UP,   .extId = 0 },
            { .name = "in-0/2",  .type = PROF_TYPE_DINPUT, .pin = 1,  .mode = PROF_MODE_INPUT,  .pull = PROF_PULL_UP,   .extId = 0 },
            { .name = "irq-eth", .type = PROF_TYPE_GEN,    .pin = 40, .mode = PROF_MODE_OUTPUT, .pull = PROF_PULL_NONE, .extId = 0 },
            { .name = "led-net", .type = PROF_TYPE_GEN,    .pin = 38, .mode = PROF_MODE_OUTPUT, .pull = PROF_PULL_NONE, .extId = 0 },
            { .name = "led-sts", .type = PROF_TYPE_GEN,    .pin = 48, .mode = PROF_MODE_OUTPUT, .pull = PROF_PULL_NONE, .extId = 0 },
        },

        .onewire = {
        },

        .uart = {
        },

        .i2c = {
            { .name = "i2c-1", .sda = 8, .scl = 9 }
        },

        .spi = {
            { .name = "spi-eth", .miso = 12, .mosi = 11, .sck = 13, .ss = 35, .freq = 25 },
        },
    },

    .net = {
        .hostname = "FCPLC",
        .led = "",
        .wifi = {
            .enabled = true,
            .ssid = "FutCityPLC",
            .passwd = ""
        },
        .ethernet = {
            .enabled = false,
            .mac_addr = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xFF },
            .spi = "spi-eth",
            .irq = "irq-eth",
        },
        .gsm = {
            .uart = ""
        }
    },

    .plc = {
        .status = "led-sts",
        .alarm = "led-alrm",
        .buzzer = ""
    }
};

#endif /* __FCPLCM_2V0_HPP__ */
