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

#ifndef __ETHERNET_HPP__
#define __ETHERNET_HPP__

#include <Arduino.h>
#include <ESPAsyncWebServer.h>

#include "core/ifaces/iface.hpp"
#include "core/ifaces/spi.hpp"
#include "core/ifaces/gpio.hpp"
#include "utils/log.hpp"

#define ETH_MAC_SIZE    6

typedef enum {
    ETH_ADDR_IP,
    ETH_ADDR_GATEWAY,
    ETH_ADDR_SUBNET,
    ETH_ADDR_DNS,
    ETH_ADDR_MAX
} EthAddrType;

typedef enum {
    ETH_IF_SPI,
    ETH_IF_IRQ
} EthIfaceType;

class EthernetCardClass
{
public:
    void setEnabled(bool status);
    bool getEnabled() const;
    void setAddress(EthAddrType type, const String &addr);
    void setInterface(EthIfaceType type, Interface *iface);
    Interface *getInterface(EthIfaceType type);
    IPAddress getAddress(EthAddrType type) const;
    void setMAC(byte *mac);
    String getMAC() const;
    bool getDHCP() const;
    void setDHCP(bool status);
    void begin();
    bool getStatus() const;
    String getHostname() const;
    void setHostname(const String &name);
    bool isFullDuplex();
    unsigned getSpeed();

private:
    bool        _enabled = false;
    IPAddress   _addr[ETH_ADDR_MAX];
    byte        _mac[ETH_MAC_SIZE] = { 0 };
    bool        _dhcp = true;
    IfSPI     *_spi = nullptr;
    IfGPIO   *_irq = nullptr;
};

extern EthernetCardClass EthernetCard;

#endif /* __ETHERNET_HPP__ */
