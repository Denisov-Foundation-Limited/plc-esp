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

#ifndef __BOARDS_HPP__
#define __BOARDS_HPP__

#define BOARD_FCPLC_3v0

#ifdef BOARD_FCPLCM_1v0
#include "profiles/fcplcm_1v0_profile.hpp"
#endif
#ifdef BOARD_FCPLCM_2v0
#include "profiles/fcplcm_2v0_profile.hpp"
#endif
#ifdef BOARD_FCPLC_3v0
#include "profiles/fcplc_3v0_profile.hpp"
#endif

#endif /* __BOARDS_HPP__ */
