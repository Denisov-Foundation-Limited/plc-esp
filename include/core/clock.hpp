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

#ifndef __CLOCK_HPP__
#define __CLOCK_HPP__

#include <GyverDS3231.h>

#define CLOCK_DEFAULT_UTC   7
#define CLOCK_UTC_MIN       1
#define CLOCK_UTC_MAX       12

class ClockClass
{
public:
    void begin();
    float getTemp();
    void getTime(Datime &time);
    bool setTime(Datime &time);
    bool setTimeUTC(Datime &time);
    uint8_t &getUTC();
    void setUTC(uint8_t utc);

private:
    GyverDS3231 _ds;
    uint8_t     _utc = CLOCK_DEFAULT_UTC;
};

extern ClockClass Clock;

#endif /* __CLOCK_HPP__ */
