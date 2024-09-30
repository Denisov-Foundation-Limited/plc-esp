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

#include "net/webgui.hpp"
#include "utils/configs.hpp"
#include "net/core/eth.hpp"
#include "net/core/wifi.hpp"
#include "controllers/meteo/meteo.hpp"
#include "controllers/socket/socket.hpp"

#include <StringUtils.h>

/*********************************************************************/
/*                                                                   */
/*                          PUBLIC FUNCTIONS                         */
/*                                                                   */
/*********************************************************************/

void WebGUIClass::begin()
{
    onBuild([this](sets::Builder& b) {
        _buildMenu(b);

        switch (_curPage) {
            case WEB_PAGE_MAIN:
                _buildMainPage(b);
                break;
            case WEB_PAGE_TELEGRAM:
                _buildTgBotPage(b);
                break;
            case WEB_PAGE_CONTROLLERS:
                _buildCtrlsPage(b);
                break;
            case WEB_PAGE_SETTINGS:
                _buildSettingsPage(b);
                break;
            case WEB_PAGE_SOCKETS:
                _buildSocketsPage(b);
                break;
        }
    });

    onUpdate([this](sets::Updater& upd) {
        switch (_curPage) {
            case WEB_PAGE_MAIN:
                _updateMainPage(upd);
                break;
            case WEB_PAGE_TELEGRAM:
                _updateTgBotPage(upd);
                break;
            case WEB_PAGE_CONTROLLERS:
                break;
            case WEB_PAGE_SETTINGS:
                break;
            case WEB_PAGE_SOCKETS:
                _updateSocketsPage(upd);
                break;
        }
    });

    setTitle(F("Future City PLC"));
    setPass(_password);
    setUpdatePeriod(500);
    SettingsAsync::begin();
}

void WebGUIClass::loop()
{
    tick();
}

void WebGUIClass::setPassword(const String &pwd)
{
    _password = pwd;
}

const String &WebGUIClass::getPassword()
{
    return _password;
}

/*********************************************************************/
/*                                                                   */
/*                          PRIVATE FUNCTIONS                        */
/*                                                                   */
/*********************************************************************/

void WebGUIClass::_buildMenu(sets::Builder& b)
{
    if (b.beginGroup(F("Меню"))) {
        b.beginButtons();
            if (b.Button("menu_btn_net"_h, F("Сеть"), sets::Colors::Aqua)) {
            _curPage = WEB_PAGE_MAIN;
            b.reload();
        }
        if (b.Button("menu_btn_tg"_h, F("Telegram"), sets::Colors::Aqua)) {
            _curPage = WEB_PAGE_TELEGRAM;
            b.reload();
        }
        if (b.Button("menu_btn_ctrl"_h, F("Контроллеры"), sets::Colors::Aqua)) {
            _curPage = WEB_PAGE_CONTROLLERS;
            _ctrl.Error = "";
            b.reload();
        }
        b.endButtons();
        b.beginButtons();
        if (b.Button("menu_btn_set"_h, F("Система"), sets::Colors::Aqua)) {
            _curPage = WEB_PAGE_SETTINGS;
            b.reload();
        }
        if (b.Button("menu_btn_set"_h, F("Настройки"), sets::Colors::Aqua)) {
            _curPage = WEB_PAGE_SETTINGS;
            b.reload();
        }
        b.endButtons();
        
        b.endGroup();
    }
}

void WebGUIClass::_buildMainPage(sets::Builder& b)
{
    if (b.beginGroup(F("Проводная сеть"))) {
        if (b.Switch("main_eth_en"_h, F("Enabled"), String(Ethernet.getEnabled()))) {
            Ethernet.setEnabled(b.build().value().toBool());
            if (Ethernet.getEnabled()) {
                Wireless.setEnabled(false);
            }
            b.reload();
        }
        if (Ethernet.getEnabled()) {
            if (b.Input("main_eth_host"_h, F("Hostname"), Ethernet.getHostname())) {
                Ethernet.setHostname(b.build().value().toString());
            }
            if (b.Switch("main_eth_dhcp"_h, F("DHCP"), String(Ethernet.getDHCP()))) {
                Ethernet.setDHCP(b.build().value().toBool());
            }
            if (b.Input("main_eth_ip"_h, F("IP"), Ethernet.getAddress(ETH_ADDR_IP).toString())) {
                Ethernet.setAddress(ETH_ADDR_IP, b.build().value().toString());
            }
            if (b.Input("main_eth_sn"_h, F("Subnet"), Ethernet.getAddress(ETH_ADDR_SUBNET).toString())) {
                Ethernet.setAddress(ETH_ADDR_SUBNET, b.build().value().toString());
            }
            if (b.Input("main_eth_gw"_h, F("Gateway"), Ethernet.getAddress(ETH_ADDR_GATEWAY).toString())) {
                Ethernet.setAddress(ETH_ADDR_GATEWAY, b.build().value().toString());
            }
            if (b.Input("main_eth_dns"_h, F("DNS"), Ethernet.getAddress(ETH_ADDR_DNS).toString())) {
                Ethernet.setAddress(ETH_ADDR_DNS, b.build().value().toString());
            }
            b.LED(F("Connection"), String(Ethernet.getStatus()));
            b.Label(F("Duplex"), Ethernet.isFullDuplex() ? F("Full") : F("Half"));
            b.Label(F("Speed"), Ethernet.getSpeed() + String(F("Mb/s")));
        }
        b.endGroup();
    }

    if (b.beginGroup(F("Беспроводная сеть"))) {
        if (b.Switch("main_wf_en"_h, F("Enabled"), String(Wireless.getEnabled()))) {
            Wireless.setEnabled(b.build().value().toBool());
            if (Wireless.getEnabled()) {
                Ethernet.setEnabled(false);
            }
            b.reload();
        }
        if (Wireless.getEnabled()) {
            if (b.Input("main_wf_ssid"_h, F("SSID"), Wireless.getSSID())) {
                Wireless.setCreds(b.build().value(), Wireless.getPasswd());
            }
            if (b.Pass("main_wf_pwd"_h, F("Password"), Wireless.getPasswd())) {
                Wireless.setCreds(Wireless.getSSID(), b.build().value());
            }
            if (b.Switch("main_wf_ap"_h, F("AP"), String(Wireless.getAP()))) {
                Wireless.setAP(b.build().value().toBool());
            }
            b.Label(F("IP"), Wireless.getIP());
            b.LED(F("Connection"), String(Wireless.getStatus() == WL_CONNECTED));
        }
        b.endGroup();
    }

    b.Label(F("Copyright"), F("Denisov Foundation Limited 2024"));
}

void WebGUIClass::_updateMainPage(sets::Updater& upd)
{
    upd.update("main_eth_ip"_h, Ethernet.getAddress(ETH_ADDR_IP).toString());
    upd.update("main_eth_sn"_h, Ethernet.getAddress(ETH_ADDR_SUBNET).toString());
    upd.update("main_eth_gw"_h, Ethernet.getAddress(ETH_ADDR_GATEWAY).toString());
    upd.update("main_eth_dns"_h, Ethernet.getAddress(ETH_ADDR_DNS).toString());
}

void WebGUIClass::_buildTgBotPage(sets::Builder& b)
{
    if (b.beginGroup(F("Telegram"))) {
        if (b.Switch("tg_en"_h, F("Enabled"), String(TgBot.getEnabled()))) {
            TgBot.setEnabled(b.build().value().toBool());
            b.reload();
        }
        if (TgBot.getEnabled()) {
            if (b.Pass("tg_key"_h, F("Token"), TgBot.getToken())) {
                TgBot.setToken(b.build().value());
            }
            auto mode = TgBot.getPollMode();
            uint8_t modeNum = 0;
            switch (mode) {
                case fb::Poll::Long:
                    modeNum = 0;
                    break;
                case fb::Poll::Sync:
                    modeNum = 1;
                    break;
                case fb::Poll::Async:
                    modeNum = 2;
                    break;
            }
            if (b.Select("tg_mod"_h, F("Mode"), F("Long;Sync;Async"), String(modeNum))) {
                switch (b.build().value().toInt32()) {
                    case 0:
                        TgBot.setPollMode(fb::Poll::Long, TgBot.getPollPeriod());
                        break;
                    case 1:
                        TgBot.setPollMode(fb::Poll::Sync, TgBot.getPollPeriod());
                        break;
                    case 2:
                        TgBot.setPollMode(fb::Poll::Async, TgBot.getPollPeriod());
                        break;
                }
            }
            if (b.Slider("tg_period"_h, "Period", 100, 100000, 10, F("msec"), String(TgBot.getPollPeriod()))) {
                TgBot.setPollMode(TgBot.getPollMode(), b.build().value().toInt32());
            }
        }
        b.endGroup();

        if (b.beginMenu("Пользователи")) {
            if (b.beginGroup(F("Выбрать"))) {
                String users = "";
                size_t last = TgBot.getUsers().size() - 1;
                size_t i = 0;
                for (auto *user : TgBot.getUsers()) {
                    users += user->name;
                    if (i < last) {
                        users += ";";
                    }
                    i++;
                }
                if (b.Select("tg_susr"_h, F("Name"), users, String(0))) {
                    auto *user = TgBot.getUsers()[b.build().value().toInt32()];
                    if (user != nullptr) {
                        _newTgUser.name = user->name;
                        _newTgUser.chatId = user->chatId;
                        _newTgUser.notify = user->notify;
                        _newTgUser.admin = user->admin;
                    }
                }
                b.endGroup();
            }

            if (b.beginGroup(F("Редактировать"))) {
                if (b.Input("tg_nusr_name"_h, F("Name"), _newTgUser.name)) {
                    _newTgUser.name = b.build().value();
                }
                if (b.Input("tg_nusr_id"_h, F("ChatID"), String(_newTgUser.chatId))) {
                    _newTgUser.chatId = b.build().value().toInt32();
                }
                if (b.Switch("tg_nusr_ntf"_h, F("Notify"), String(_newTgUser.notify))) {
                    _newTgUser.notify = b.build().value().toBool();
                }
                if (b.Switch("tg_nusr_adm"_h, F("Admin"), String(_newTgUser.admin))) {
                    _newTgUser.admin = b.build().value().toBool();
                }
                b.beginButtons();
                if (b.Button("tg_nusr_add"_h, F("Добавить"))) {
                    auto user = new TgUser();
                    user->name = _newTgUser.name;
                    user->chatId = _newTgUser.chatId;
                    user->notify = _newTgUser.notify;
                    user->admin = _newTgUser.admin;
                    user->level = TG_MENU_MAIN;
                    TgBot.addUser(user);
                    _newTgUser.name = "";
                    _newTgUser.notify = false;
                    _newTgUser.admin = false;
                    _newTgUser.chatId = 0;
                    b.reload();
                }
                if (b.Button("tg_nusr_upd"_h, F("Обновить"), sets::Colors::Aqua)) {
                    auto user = TgBot.getUser(_newTgUser.name);
                    if (user != nullptr) {
                        user->name = _newTgUser.name;
                        user->chatId = _newTgUser.chatId;
                        user->notify = _newTgUser.notify;
                        user->admin = _newTgUser.admin;
                    }
                    b.reload();
                }
                if (b.Button("tg_nusr_rm"_h, F("Удалить"), sets::Colors::Red)) {
                    TgBot.removeUser(_newTgUser.name);
                }
                b.endButtons();
                b.endGroup();

                size_t i = 1;
                for (auto *user : TgBot.getUsers()) {
                    if (b.beginGroup("Пользователь #" + String(i))) {
                        b.Label(F("Name"), user->name);
                        b.Label(F("ChatID"), String(user->chatId));
                        b.Switch("tg_usr_ntf"_h, F("Notify"), String(user->notify));
                        b.Switch("tg_usr_adm"_h, F("Admin"), String(user->admin));
                        b.endGroup();
                    }
                    i++;
                }
            }
            b.endMenu();
        }
    }
}

void WebGUIClass::_updateTgBotPage(sets::Updater& upd)
{
    upd.update("tg_nusr_name"_h, _newTgUser.name);
    upd.update("tg_nusr_id"_h, String(_newTgUser.chatId));
    upd.update("tg_nusr_ntf"_h, String(_newTgUser.notify));
    upd.update("tg_nusr_adm"_h, String(_newTgUser.admin));
}

void WebGUIClass::_buildCtrlsPage(sets::Builder& b)
{
    if (b.beginMenu(F("Настройки"))) {
        if (b.beginGroup(F("Настройки"))) {
            if (b.Input("ctrl_name"_h, "Имя", _ctrl.Name)) {
                _ctrl.Name = b.build().value();            
            }
            if (b.Select("ctrl_select"_h, "Тип", "Метео;Охрана;Бак;Розетки", String(_ctrl.Type))) {
                _ctrl.Type = static_cast<CtrlType>(b.build().value().toInt32());
            }
            if (b.Button("ctrl_add"_h, F("Добавить"))) {
                if (_ctrl.Name == "") {
                    _ctrl.Error = F("Пустое имя");
                } else if (Controllers.isExists(_ctrl.Name)) {
                    _ctrl.Error = F("Имя занято");
                } else {
                    switch (_ctrl.Type) {
                        case CTRL_TYPE_METEO:
                            Controllers.addController(new MeteoCtrl(_ctrl.Name));
                            break;
                        case CTRL_TYPE_SOCKET:
                            Controllers.addController(new SocketCtrl(_ctrl.Name));
                            break;
                    }
                    _ctrl.Name = "";
                    _ctrl.Error = "";
                }
                b.reload();
            }
            b.endGroup();
        }
        b.endMenu();
    }

    if (_ctrl.Error != "") {
        b.Label(F("Ошибка"), _ctrl.Error, sets::Colors::Red);
    }

    if (b.beginGroup(F("Контроллеры"))) {
        for (auto *ctrl : Controllers.getControllers()) {
            b.beginButtons();
            if (b.Button(su::SH(("ctrl_" + ctrl->getName()).c_str()), ctrl->getName())) {
                switch (ctrl->getType()) {
                    case CTRL_TYPE_SOCKET:
                        _curPage = WEB_PAGE_SOCKETS;
                        _ctrl.Name = ctrl->getName();
                        break;
                }
                b.reload();
            }
            b.endButtons();
        }
        b.endGroup();
    }
}

void WebGUIClass::_helperSocketsPage()
{
    std::vector<Interface *> inputs;
    std::vector<Interface *> outputs;

    auto *sock = static_cast<SocketCtrl *>(Controllers.getController(_ctrl.Name));
    _socket.curSock = sock->getSockets().size();

    Interfaces.getInterfacesByType(outputs, INT_TYPE_RELAY);
    Interfaces.getInterfacesByType(inputs, INT_TYPE_DIGITAL_INPUT);

    _socket.curRly = outputs.size();
    _socket.curBtn = inputs.size();

    for (size_t i = 0; i < sock->getSockets().size(); i++) {
        if (sock->getSockets()[i]->getName() == _socket.EdName) {
            _socket.curSock = i;
        }
    }

    for (size_t i = 0; i < outputs.size(); i++) {
        if (_socket.Relay == outputs[i]->getName()) {
            _socket.curRly = i;
        }
    }

    for (size_t i = 0; i < inputs.size(); i++) {
        if (_socket.Button == inputs[i]->getName()) {
            _socket.curBtn = i;
        }
    }
}
void WebGUIClass::_updateSocketsPage(sets::Updater& upd)
{
    _helperSocketsPage();
    upd.update("ctrl_sock_sel"_h, String(_socket.curSock));
    upd.update("ctrl_sock_erly"_h, String(_socket.curRly));
    upd.update("ctrl_sock_ebtn"_h, String(_socket.curBtn));
    upd.update("ctrl_sock_ename"_h, (_socket.NewName != "") ? _socket.NewName : _socket.EdName);
}

void WebGUIClass::_buildSocketsPage(sets::Builder& b)
{
    _helperSocketsPage();

    b.Label(F("Контроллер"), _ctrl.Name);

    if (b.beginMenu(F("Настройки"))) {
        if (b.beginGroup(F("Добавить Розетку"))) {
            if (b.Input("ctrl_sock_name"_h, "Имя", _socket.AddName)) {
                _socket.AddName = b.build().value();
            }
            if (b.Button("ctrl_sock_add"_h, F("Добавить"))) {
                auto *sock = static_cast<SocketCtrl *>(Controllers.getController(_ctrl.Name));
                if (_socket.AddName == "") {
                    _socket.AddError = F("Пустое имя");
                } else if (sock->isExists(_socket.AddName)) {
                    _socket.AddError = F("Имя занято");
                } else {
                    sock->addSocket(new Socket(_socket.AddName));
                    _socket.AddName = "";
                    _socket.AddError = "";
                }
                b.reload();
            }
            b.endGroup();
        }

        if (b.beginGroup(F("Редактировать"))) {
            String                      sSockets = "";
            String                      sIn = "", sOut = "";
            std::vector<Interface *>    inputs;
            std::vector<Interface *>    outputs;

            auto *sock = static_cast<SocketCtrl *>(Controllers.getController(_ctrl.Name));

            Interfaces.getInterfacesByType(outputs, INT_TYPE_RELAY);
            Interfaces.getInterfacesByType(inputs, INT_TYPE_DIGITAL_INPUT);

            for (size_t i = 0; i < sock->getSockets().size(); i++) {
                sSockets += sock->getSockets()[i]->getName() + ";";
            }

            for (size_t i = 0; i < outputs.size(); i++) {
                sOut += outputs[i]->getName() + ";";
            }

            for (size_t i = 0; i < inputs.size(); i++) {
                sIn += inputs[i]->getName() + ";";
            }

            if (b.Select("ctrl_sock_sel"_h, F("Розетка"), sSockets, String(_socket.curSock))) {
                auto value = b.build().value().toInt32();
                if (sock->getSockets().size() > 0 && value < sock->getSockets().size()) {
                    auto s = sock->getSockets()[value];
                    if (s != nullptr) {
                        _socket.EdName = s->getName();
                        if (s->getGpio(SOCK_IF_BUTTON) != nullptr) {
                            _socket.Button = s->getGpio(SOCK_IF_BUTTON)->getName();
                        } else {
                            _socket.Button = "";
                        }
                        if (s->getGpio(SOCK_IF_RELAY) != nullptr) {
                            _socket.Relay = s->getGpio(SOCK_IF_RELAY)->getName();
                        } else {
                            _socket.Relay = "";
                        }
                    }
                }
            }

            if (b.Input("ctrl_sock_ename"_h, F("Имя"), _socket.EdName)) {
                _socket.NewName = b.build().value();
            }
            if (b.Select("ctrl_sock_erly"_h, F("Реле"), sOut, String(_socket.curRly))) {
                auto value = b.build().value().toInt32();
                if (value == outputs.size()) {
                    _socket.Relay = "";
                } else {
                    auto iface = outputs[value];
                    _socket.Relay = iface != nullptr ? iface->getName() : "";
                }
            }
            if (b.Select("ctrl_sock_ebtn"_h, F("Кнопка"), sIn, String(_socket.curBtn))) {
                auto value = b.build().value().toInt32();
                if (value == inputs.size()) {
                    _socket.Button = "";
                } else {
                    auto iface = inputs[value];
                    _socket.Button = iface != nullptr ? iface->getName() : "";
                }
            }
            if (b.Button("ctrl_sock_e"_h, F("Изменить"))) {
                auto s = sock->getSocket(_socket.EdName);
                if (s != nullptr) {
                    s->setGpio(SOCK_IF_RELAY, (_socket.Relay == "") ? nullptr : Interfaces.getInterface(_socket.Relay));
                    s->setGpio(SOCK_IF_BUTTON, (_socket.Button == "") ? nullptr : Interfaces.getInterface(_socket.Button));
                    if (_socket.NewName != "") {
                        s->setName(_socket.NewName);
                        _socket.EdName = _socket.NewName;
                        _socket.NewName = "";
                    }
                    b.reload();
                }
            }
            b.endGroup();

            if (_socket.EdError != "") {
                b.Label(F("Ошибка"), _socket.EdError, sets::Colors::Red);
            }
        }
        b.endMenu();
    }

    if (b.beginGroup(F("Общее"))) {
        b.beginButtons();           
        if (b.Button(su::SH(String("ctrl_sock_onall").c_str()), F("Включить все"))) {
            auto *sock = static_cast<SocketCtrl *>(Controllers.getController(_ctrl.Name));
            for (auto s : sock->getSockets()) {
                s->setStatus(true, true);
            }
            b.reload();
        }
        if (b.Button(su::SH(String("ctrl_sock_offall").c_str()), F("Отключить все"), sets::Colors::Red)) {
            auto *sock = static_cast<SocketCtrl *>(Controllers.getController(_ctrl.Name));
            for (auto s : sock->getSockets()) {
                s->setStatus(false, true);
            }
            b.reload();
        }
        b.endButtons();

        b.endGroup();
    }

    auto *sock = static_cast<SocketCtrl *>(Controllers.getController(_ctrl.Name));
    for (auto s : sock->getSockets()) {
        if (b.beginGroup(s->getName())) {
            b.LED(F("Статус"), s->getStatus());
            b.beginButtons();
            if (b.Button(su::SH(String("ctrl_sock_on_" + s->getName()).c_str()), F("Включить"))) {
                s->setStatus(true, true);
                b.reload();
            }
            if (b.Button(su::SH(String("ctrl_sock_off_" + s->getName()).c_str()), F("Отключить"), sets::Colors::Red)) {
                s->setStatus(false, true);
                b.reload();
            }
            b.endButtons();
            b.endGroup();
        }
    }       
}

void WebGUIClass::_buildSettingsPage(sets::Builder& b)
{
    if (b.beginGroup(F("Настройки"))) {
        b.beginButtons();
        if (b.Button("set_sv"_h, F("Сохранить"))) {
            Configs.writeAll();
            b.reload();
        }
        if (b.Button("set_rm"_h, F("Удалить"), sets::Colors::Red)) {
            Configs.eraseAll();
            b.reload();
        }
        b.endButtons();
        b.endGroup();
    }

    if (b.beginGroup(F("Система"))) {
        if (b.Button("set_res"_h, F("Рестарт"))) {
            ESP.restart();
        }
        b.endGroup();
    }
}

WebGUIClass WebGUI;
