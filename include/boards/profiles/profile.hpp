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

typedef struct {
    unsigned id;
    unsigned addr;
} ExtData;

typedef struct {
    unsigned rx;
    unsigned tx;
    unsigned io;
} RS485Pins;

typedef struct {
    unsigned pin;
    unsigned extId;
} ExtGpio;

#endif /* __PROFILE_HPP__ */
