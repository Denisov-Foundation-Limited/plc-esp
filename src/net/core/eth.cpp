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

#include "net/core/eth.hpp"
#include "net/core/wifi.hpp"
#include "Ethernet.h"

void EthernetCardClass::setEnabled(bool status)
{
    _enabled = status;
}

bool EthernetCardClass::getEnabled() const
{
    return _enabled;
}

void EthernetCardClass::setAddress(EthAddrType type, const String &addr)
{
    _addr[type].fromString(addr.c_str());
}

void EthernetCardClass::setInterface(EthIfaceType type, Interface *iface)
{
    switch (type) {
        case ETH_IF_SPI:
            _spi = static_cast<IfSPI *>(iface);
            break;
        case ETH_IF_IRQ:
            _irq = static_cast<IfGPIO *>(iface);
            break;
    }
}

Interface *EthernetCardClass::getInterface(EthIfaceType type)
{
    switch (type) {
        case ETH_IF_SPI:
            return _spi;
        case ETH_IF_IRQ:
            return _irq;
    }
    return nullptr;
}

IPAddress EthernetCardClass::getAddress(EthAddrType type) const
{
    if (!_dhcp) {
        return _addr[type];
    } else {
        switch (type) {
            case ETH_ADDR_IP:
                return IPAddress(0,0,0,0);
                //;return Ethernet.localIP();
            case ETH_ADDR_GATEWAY:
                return IPAddress(0,0,0,0);//return Ethernet.gatewayIP();
            case ETH_ADDR_SUBNET:
                return IPAddress(0,0,0,0);//return Ethernet.subnetMask();
            case ETH_ADDR_DNS:
                return IPAddress(0,0,0,0);//return Ethernet.dnsIP();
        }
    }
    return IPAddress(0,0,0,0);
}

void EthernetCardClass::setMAC(byte *mac)
{
    for (int i = 0; i < ETH_MAC_SIZE; i++) {
        _mac[i] = mac[i];
    }
}

String EthernetCardClass::getMAC() const
{
    String mac = "";

    for (auto b : _mac) {
        if (b < 0x10) {
            mac += "0";
        }
        mac += String(b, HEX);
    }
    mac.toUpperCase();

    return mac;
}

bool EthernetCardClass::getDHCP() const
{
    return _dhcp;
}

void EthernetCardClass::setDHCP(bool status)
{
    _dhcp = status;
}

void EthernetCardClass::begin()
{
    if (!_enabled) return;
    if (_mac[0] = 0x0) return;

    if (_spi == nullptr) {
        Log.error(LOG_MOD_ETH, F("Ethernet SPI interface not found"));
        return;
    }
    if (_irq == nullptr) {
        Log.error(LOG_MOD_ETH, F("Ethernet IRQ interface not found"));
        return;
    }

    Log.info(LOG_MOD_ETH, F("Connecting to wired network"));

    //ESP32_W5500_onEvent();

    /*if (!Ethernet.begin( _spi->getPin(SPI_PIN_MISO), _spi->getPin(SPI_PIN_MOSI), _spi->getPin(SPI_PIN_SCK),
                _spi->getPin(SPI_PIN_SS), _irq->getPin(), _spi->getFrequency(), SPI3_HOST, _mac)) {
        Log.error(LOG_MOD_ETH, F("Ethernet init error"));
        return;
    }
    if (!_dhcp) {
        Ethernet.config(_addr[ETH_ADDR_IP], _addr[ETH_ADDR_GATEWAY], _addr[ETH_ADDR_SUBNET], _addr[ETH_ADDR_DNS]);
    }
    if (!ESP32_W5500_waitForConnect()) {
        Log.error(LOG_MOD_ETH, String(F("Failed to connect to wired network")));
        EthernetCard.setEnabled(false);
        Wireless.setAP(true);
        Wireless.setEnabled(true);
        Wireless.begin();
    } else {
        Log.info(LOG_MOD_ETH, String(F("Connected. IP: ")) + getAddress(ETH_ADDR_IP).toString() +
                            String(F(" Duplex: ")) + String("Full") +
                            String(F(" Speed: ")) +  String(F("Mbps")));
    }*/

   Ethernet.init(_spi->getPin(SPI_PIN_SS));
   Ethernet.begin(_mac);
   
}

bool EthernetCardClass::isFullDuplex()
{
    return true; //return Ethernet.fullDuplex();
}

unsigned EthernetCardClass::getSpeed()
{
    return 0;//return Ethernet.linkSpeed();
}

bool EthernetCardClass::getStatus() const
{
    return true; //return ESP32_W5500_isConnected();
}

String EthernetCardClass::getHostname() const
{
    return "";//return ESP32_W5500_get_hostname();
}

void EthernetCardClass::setHostname(const String &name)
{
    //ESP32_W5500_set_hostname(name);
}

EthernetCardClass EthernetCard;
