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

#include "boards/profiles/fcplc_3v0_profile.hpp"
#include "boards/profiles/profile.hpp"

BoardProfile PROGMEM ActiveBoard = {
    .name = "FCPLC-3v0",
    .serial_rate = 115200,
    
    .interfaces = {
        .i2c = {
            { .id = 1, .sda = 2,  .scl = 1 },
            { .id = 2, .sda = 47, .scl = 48 }
        },

        .ext = {
            { .id = 1,  .i2c = 1, .addr = 0x20 },
            { .id = 2,  .i2c = 1, .addr = 0x21 },
            { .id = 3,  .i2c = 2, .addr = 0x20 },
            { .id = 4,  .i2c = 2, .addr = 0x21 },
            { .id = 5,  .i2c = 2, .addr = 0x22 },
            { .id = 6,  .i2c = 2, .addr = 0x23 },
            { .id = 7,  .i2c = 2, .addr = 0x24 },
            { .id = 8,  .i2c = 2, .addr = 0x25 },
            { .id = 9,  .i2c = 2, .addr = 0x26 },
            { .id = 10, .i2c = 2, .addr = 0x27 }
        },

        .gpio = {
            { .id = 1,  .pin = 15, .type = PROF_GPIO_SENSOR,  .ext = 0 },
            { .id = 2,  .pin = 16, .type = PROF_GPIO_SENSOR,  .ext = 0 },
            { .id = 3,  .pin = 17, .type = PROF_GPIO_SENSOR,  .ext = 0 },
            { .id = 4,  .pin = 18, .type = PROF_GPIO_SENSOR,  .ext = 0 },
            { .id = 5,  .pin = 8,  .type = PROF_GPIO_SENSOR,  .ext = 0 },
            { .id = 6,  .pin = 9,  .type = PROF_GPIO_SENSOR,  .ext = 0 },
            { .id = 7,  .pin = 38, .type = PROF_GPIO_BUZZER,  .ext = 0 },
            { .id = 8,  .pin = 39, .type = PROF_GPIO_GENERIC, .ext = 0 },
            { .id = 9,  .pin = 7,  .type = PROF_GPIO_RELAY,   .ext = 1 },
            { .id = 10, .pin = 6,  .type = PROF_GPIO_RELAY,   .ext = 1 },
            { .id = 11, .pin = 5,  .type = PROF_GPIO_RELAY,   .ext = 1 },
            { .id = 12, .pin = 4,  .type = PROF_GPIO_RELAY,   .ext = 1 },
            { .id = 13, .pin = 3,  .type = PROF_GPIO_RELAY,   .ext = 1 },
            { .id = 14, .pin = 2,  .type = PROF_GPIO_RELAY,   .ext = 1 },
            { .id = 15, .pin = 1,  .type = PROF_GPIO_RELAY,   .ext = 1 },
            { .id = 16, .pin = 15, .type = PROF_GPIO_RELAY,   .ext = 1 },
            { .id = 17, .pin = 0,  .type = PROF_GPIO_INPUT,   .ext = 1 },
            { .id = 18, .pin = 8,  .type = PROF_GPIO_INPUT,   .ext = 1 },
            { .id = 19, .pin = 9,  .type = PROF_GPIO_INPUT,   .ext = 1 },
            { .id = 20, .pin = 10, .type = PROF_GPIO_INPUT,   .ext = 1 },
            { .id = 21, .pin = 11, .type = PROF_GPIO_INPUT,   .ext = 1 },
            { .id = 22, .pin = 12, .type = PROF_GPIO_INPUT,   .ext = 1 },
            { .id = 23, .pin = 13, .type = PROF_GPIO_INPUT,   .ext = 1 },
            { .id = 24, .pin = 14, .type = PROF_GPIO_INPUT,   .ext = 1 },
            { .id = 25, .pin = 0,  .type = PROF_GPIO_GENERIC, .ext = 2 },
            { .id = 26, .pin = 1,  .type = PROF_GPIO_GENERIC, .ext = 2 },
            { .id = 27, .pin = 2,  .type = PROF_GPIO_GENERIC, .ext = 2 },
            { .id = 28, .pin = 3,  .type = PROF_GPIO_GENERIC, .ext = 2 },
            { .id = 29, .pin = 4,  .type = PROF_GPIO_GENERIC, .ext = 2 },
            { .id = 30, .pin = 5,  .type = PROF_GPIO_GENERIC, .ext = 2 },
            { .id = 31, .pin = 6,  .type = PROF_GPIO_GENERIC, .ext = 2 },
            { .id = 32, .pin = 7,  .type = PROF_GPIO_GENERIC, .ext = 2 },
            { .id = 33, .pin = 8,  .type = PROF_GPIO_GENERIC, .ext = 2 },
            { .id = 34, .pin = 9,  .type = PROF_GPIO_GENERIC, .ext = 2 },
            { .id = 35, .pin = 10, .type = PROF_GPIO_GENERIC, .ext = 2 },
            { .id = 36, .pin = 11, .type = PROF_GPIO_GENERIC, .ext = 2 },
            { .id = 37, .pin = 12, .type = PROF_GPIO_GENERIC, .ext = 2 },
            { .id = 38, .pin = 13, .type = PROF_GPIO_GENERIC, .ext = 2 },
            { .id = 39, .pin = 14, .type = PROF_GPIO_GENERIC, .ext = 2 },
            { .id = 40, .pin = 15, .type = PROF_GPIO_GENERIC, .ext = 2 },
        },

        .spi = {
            { .id = 0, .miso = 12, .mosi = 11, .sck = 13, .ss = 10 }
        },

        .ow = {
            { .id = 1, .pin = 6 },
            { .id = 2, .pin = 7 }
        },

        .uart = {
            { .id = 1, .rx = 4, .tx = 5, .speed = 115200 }
        }
    },

    .plc = {
        .gpio = {
            .fan = 26,
            .status = 31,
            .alarm = 30,
            .relays = {
                40, 39, 38, 37, 36, 35, 34, 33
            },
            .up = 29,
            .middle = 28,
            .down = 27
        },
        .temp = {
            .i2c = 1,
            .addr = 0x48
        }
    },

   /* .plc = {
        .gpio = {
            .buttons = {
                
            },
            .fan = 26,
            .led = {
                
                
            },
            .lcd = {
                .light = 25
            }
        },
        .lcd = {
            .i2c = 1,
            .addr = 0x22
        },
        .temp = {
            .i2c = 1,
            .addr = 0x48
        },
    },*/

    .wifi = {
        .enabled = true,
        .ssid = "FCPLC-3v0",
        .passwd = "",
        .hostname = "FCPLC",
        .gpio = {
            .net = 32
        }
    }
};
