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

class EthernetClass
{
public:
    void setEnabled(bool status)
    {
        _enabled = _enabled;
    }

    bool getEnabled() const
    {
        return _enabled;
    }

    void setAddress(EthAddrType type, const String &addr)
    {
        _addr[type].fromString(addr.c_str());
    }

    void setInterface(EthIfaceType type, Interface *iface)
    {
        switch (type) {
            case ETH_IF_SPI:
                _spi = static_cast<SPIface *>(iface);
                break;
            case ETH_IF_IRQ:
                _irq = static_cast<GPIOIface *>(iface);
                break;
        }
    }

    Interface *getInterface(EthIfaceType type)
    {
        switch (type) {
            case ETH_IF_SPI:
                return _spi;
            case ETH_IF_IRQ:
                return _irq;
        }
    }

    IPAddress getAddress(EthAddrType type) const
    {
        if (!_dhcp) {
            return _addr[type];
        } else {
            switch (type) {
                case ETH_ADDR_IP:
                    return ETH.localIP();
                case ETH_ADDR_GATEWAY:
                    return ETH.gatewayIP();
                case ETH_ADDR_SUBNET:
                    return ETH.subnetMask();
                case ETH_ADDR_DNS:
                    return ETH.dnsIP();
            }
        }
        return IPAddress(0,0,0,0);
    }

    void setMAC(byte *mac)
    {
        for (int i = 0; i < ETH_MAC_SIZE; i++) {
            _mac[i] = mac[i];
        }
    }

    String getMAC() const
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

    bool getDHCP() const
    {
        return _dhcp;
    }

    void setDHCP(bool status)
    {
        _dhcp = status;
    }

    void begin()
    {
        if (!_enabled) return;
        if (_mac[0] = 0x0) return;

        if (_spi == nullptr) {
            Log.error(LOG_MOD_ETH, "Ethernet SPI interface not found");
            return;
        }
        if (_irq == nullptr) {
            Log.error(LOG_MOD_ETH, "Ethernet IRQ interface not found");
            return;
        }

        ESP32_W5500_onEvent();
        if (!_dhcp) {
            ETH.config(_addr[ETH_ADDR_IP], _addr[ETH_ADDR_GATEWAY], _addr[ETH_ADDR_SUBNET], _addr[ETH_ADDR_DNS]);
        }
        if (!ETH.begin( _spi->getPin(SPI_PIN_MISO), _spi->getPin(SPI_PIN_MOSI), _spi->getPin(SPI_PIN_SCK),
                    _spi->getPin(SPI_PIN_SS), _irq->getPin(), _spi->getFrequency(), SPI3_HOST, _mac)) {
            Log.error(LOG_MOD_ETH, "Ethernet init error");
            return;
        }
    }

    bool getStatus() const
    {
        return ESP32_W5500_isConnected();
    }

    String getHostName() const
    {
        return ETH.getHostname();
    }

    void setHostname(const String &name)
    {
        ETH.setHostname(name.c_str());
    }

private:
    bool        _enabled = false;
    IPAddress   _addr[ETH_ADDR_MAX];
    byte        _mac[ETH_MAC_SIZE] = { 0 };
    bool        _dhcp = true;
    SPIface     *_spi = nullptr;
    GPIOIface   *_irq = nullptr;
};

extern EthernetClass Ethernet;

#endif /* __ETHERNET_HPP__ */
