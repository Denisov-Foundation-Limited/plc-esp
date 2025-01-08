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

#include "ftest.hpp"
#include "core/ifaces/gpio.hpp"
#include "core/ifaces/ow.hpp"

void FtestClass::start()
{
    bool                        last = false;
    std::vector<GpioPin *>      pins;
    std::vector<OneWireBus *>   ows;
    std::vector<I2cBus *>       i2cs;

    Gpio.getPins(pins);
    OneWireIf.getOWBuses(ows);
    I2C.getI2cBuses(i2cs);

    for (auto *pin : pins) {
        if (pin->type == GPIO_TYPE_RELAY || pin->type == GPIO_TYPE_GENERIC || pin->type == GPIO_TYPE_BUZZER) {
            Gpio.setMode(pin, GPIO_MOD_OUTPUT, GPIO_PULL_NONE);
        } else if (pin->type == GPIO_TYPE_INPUT) {
            Gpio.setMode(pin, GPIO_MOD_INPUT, GPIO_PULL_UP);
        }
    }

    for (;;) {
        Serial.println(F("[FTEST] ===================================================="));
        size_t i = 1;
        for (auto *pin : pins) {
            if (pin->type == GPIO_TYPE_RELAY || pin->type == GPIO_TYPE_GENERIC) {
                if (!last) {
                    Gpio.write(pin, true);
                    Serial.println("[FTEST] Relay #" + String(i) + " GPIO id #" + String(pin->id) + " status: HIGH"); 
                } else {
                    Gpio.write(pin, false);
                    Serial.println("[FTEST] Relay #" + String(i) + " GPIO id #" + String(pin->id) + " status: LOW"); 
                }
                i++;
            }
        }
        Serial.println(F("[FTEST]"));
        i = 1;
        for (auto *pin : pins) {
            if (pin->type == GPIO_TYPE_INPUT) {
                Serial.println("[FTEST] Input #" + String(i) + " GPIO id #" + String(pin->id) + " status: " + (Gpio.read(pin) ? "HIGH" : "LOW")); 
                i++;
            }
        }
        Serial.println(F("[FTEST]"));
        for (auto *bus : ows) {
            std::vector<String> addrs;
            OneWireIf.findDevices(bus, addrs);
            for (auto addr : addrs) {
                addr.toUpperCase();
                Serial.println("[FTEST] OneWire #" + String(bus->id) + " found address: " + String(addr));
            }
        }
        Serial.println(F("[FTEST]"));
        for (auto *bus : i2cs) {
            std::vector<byte> addrs;
            I2C.findDevices(bus, addrs);
            for (auto addr : addrs) {
                Serial.println("[FTEST] I2C #" + String(bus->id) + " found address: 0x" + String(addr, HEX));
            }
        }

        for (auto *pin : pins) {
            if (pin->type == GPIO_TYPE_BUZZER) {
                Gpio.write(pin, true);
            }
        }
        delay(100);
        for (auto *pin : pins) {
            if (pin->type == GPIO_TYPE_BUZZER) {
                Gpio.write(pin, false);
            }
        }

        delay(1800);
        last = !last;
    }
}

FtestClass Ftest;
