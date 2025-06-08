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

#ifndef __WEB_PAGES_HPP__
#define __WEB_PAGES_HPP__

typedef enum {
    WEB_PAGE_MAIN,
    WEB_PAGE_TELEGRAM,
    WEB_PAGE_CONTROLLERS,
    WEB_PAGE_SOCKETS,
    WEB_PAGE_SETTINGS,
    WEB_PAGE_SYSTEM,
    WEB_PAGE_METEO,
    WEB_PAGE_CLIMATE,
    WEB_PAGE_SECURITY,
    WEB_PAGE_TANK
} WebGuiPage;

#endif /* __WEB_PAGES_HPP__ */
