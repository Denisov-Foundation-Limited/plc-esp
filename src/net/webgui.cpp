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
                _updateCtrlsPage(upd);
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
    setUpdatePeriod(100);
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
        b.endButtons();
        b.beginButtons();
        if (b.Button("menu_btn_ctrl"_h, F("Контроллеры"), sets::Colors::Aqua)) {
            _curPage = WEB_PAGE_CONTROLLERS;
            _ctrl.Error = "";
            b.reload();
        }
        if (b.Button("menu_btn_set"_h, F("Система"), sets::Colors::Aqua)) {
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
            b.LED("main_con_eth"_h, F("Connection"), Ethernet.getStatus());

            String sDup, sSpd;

            if (Ethernet.getStatus()) {
                sDup = Ethernet.isFullDuplex() ? F("Full") : F("Half");
                sSpd = Ethernet.getSpeed() + String(F("Mb/s"));
            } else {
                sDup = F("None");
                sSpd = F("None");
            }

            b.Label("main_eth_dup"_h, F("Duplex"), sDup);
            b.Label("main_eth_spd"_h, F("Speed"), sSpd);
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
            b.Label("main_wf_ip"_h, F("IP"), Wireless.getIP());
            b.LED("main_con_wf"_h, F("Connection"), Wireless.getStatus() == WL_CONNECTED);
        }
        b.endGroup();
    }

    b.Label(F("Copyright"), F("Denisov Foundation Limited 2024"));
}

void WebGUIClass::_updateMainPage(sets::Updater& upd)
{
    String sDup, sSpd;

    upd.update("main_eth_ip"_h, Ethernet.getAddress(ETH_ADDR_IP).toString());
    upd.update("main_eth_sn"_h, Ethernet.getAddress(ETH_ADDR_SUBNET).toString());
    upd.update("main_eth_gw"_h, Ethernet.getAddress(ETH_ADDR_GATEWAY).toString());
    upd.update("main_eth_dns"_h, Ethernet.getAddress(ETH_ADDR_DNS).toString());
    upd.update("main_con_wf"_h, Wireless.getStatus() == WL_CONNECTED);
    upd.update("main_con_eth"_h, Ethernet.getStatus());
    upd.update("main_wf_ip"_h, Wireless.getIP());

    if (Ethernet.getStatus()) {
        sDup = Ethernet.isFullDuplex() ? F("Full") : F("Half");
        sSpd = Ethernet.getSpeed() + String(F("Mb/s"));
    } else {
        sDup = F("");
        sSpd = F("");
    }

    upd.update("main_eth_dup"_h, sDup);
    upd.update("main_eth_spd"_h, sSpd);
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
            b.Label("tg_last_id"_h, F("LastID"), String(TgBot.getLastID()));
        }
        b.endGroup();

        if (_tgUser.Error != "") {
            b.Label(F("Ошибка"), _tgUser.Error, sets::Colors::Red);
        }

        if (b.beginMenu(F("Настройки"))) {
            if (b.beginGroup(F("Добавить пользователя"))) {
                if (b.Input("tg_nuser_new_name"_h, F("Имя"), _tgUser.AddName)) {
                    _tgUser.AddName = b.build().value().toString();
                }
                if (b.Button("tg_nusr_add"_h, F("Добавить"))) {
                    if (_tgUser.AddName != "") {
                        auto user = new TgUser();
                        user->name = _tgUser.AddName;
                        user->level = TG_MENU_MAIN;
                        TgBot.addUser(user);
                        _tgUser.AddName = "";
                        _tgUser.Error = "";
                    } else {
                        _tgUser.Error = F("Пустое имя");
                    }
                    b.reload();
                }
                b.endGroup();
            }

            if (b.beginGroup(F("Редактировать"))) {
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
                if (b.Select("tg_susr"_h, F("Name"), users, String(_tgUser.curUser))) {
                    auto usrNum = b.build().value().toInt32();
                    if (usrNum < TgBot.getUsers().size()) {
                        auto *user = TgBot.getUsers()[usrNum];
                        if (user != nullptr) {
                            _tgUser.EdName = user->name;
                            _tgUser.chatID = user->chatId;
                            _tgUser.Notify = user->notify;
                            _tgUser.Admin = user->admin;
                            _tgUser.curUser = b.build().value().toInt32();
                        }
                    }
                }

                if (b.Input("tg_nusr_name"_h, F("Name"), _tgUser.EdName)) {
                    _tgUser.NewName = b.build().value();
                }
                if (b.Input("tg_nusr_id"_h, F("ChatID"), String(_tgUser.chatID))) {
                    _tgUser.chatID = b.build().value().toInt32();
                }
                if (b.Switch("tg_nusr_ntf"_h, F("Notify"), String(_tgUser.Notify))) {
                    _tgUser.Notify = b.build().value().toBool();
                }
                if (b.Switch("tg_nusr_adm"_h, F("Admin"), String(_tgUser.Admin))) {
                    _tgUser.Admin = b.build().value().toBool();
                }
                b.beginButtons();
                
                if (b.Button("tg_nusr_upd"_h, F("Изменить"), sets::Colors::Aqua)) {
                    auto user = TgBot.getUser(_tgUser.EdName);
                    if (user != nullptr) {
                        if (_tgUser.NewName != "") {
                            user->name = _tgUser.NewName;
                        }
                        user->chatId = _tgUser.chatID;
                        user->notify = _tgUser.Notify;
                        user->admin = _tgUser.Admin;
                        _tgUser.NewName = "";
                        _tgUser.Error = "";
                    }
                    b.reload();
                }
                if (b.Button("tg_nusr_rm"_h, F("Удалить"), sets::Colors::Red)) {
                    TgBot.removeUser(_tgUser.EdName);
                }
                b.endButtons();
                b.endGroup();
            }
            b.endMenu();
        }

        size_t i = 1;
        for (auto *user : TgBot.getUsers()) {
            if (b.beginGroup("Пользователь #" + String(i))) {
                b.Label(F("Имя"), user->name);
                b.Label(F("ChatID"), String(user->chatId));
                b.LED("tg_usr_ntf"_h, F("Уведомления"), user->notify);
                b.LED("tg_usr_adm"_h, F("Админ"), user->admin);
                b.endGroup();
            }
            i++;
        }
    }
}

void WebGUIClass::_updateTgBotPage(sets::Updater& upd)
{
    upd.update("tg_nuser_new_name"_h, _tgUser.AddName);
    upd.update("tg_nusr_name"_h, (_tgUser.NewName != "") ? _tgUser.NewName : _tgUser.EdName);
    upd.update("tg_nusr_id"_h, String(_tgUser.chatID));
    upd.update("tg_nusr_ntf"_h, String(_tgUser.Notify));
    upd.update("tg_nusr_adm"_h, String(_tgUser.Admin));
    upd.update("tg_susr"_h, String(_tgUser.curUser));
    upd.update("tg_last_id"_h, String(TgBot.getLastID()));
}

void WebGUIClass::_buildCtrlsPage(sets::Builder& b)
{
    if (b.beginMenu(F("Настройки"))) {
        if (b.beginGroup(F("Добавить контроллер"))) {
            if (b.Input("ctrl_name"_h, F("Имя"), _ctrl.Name)) {
                _ctrl.Name = b.build().value();            
            }
            if (b.Select("ctrl_select"_h, F("Тип"), F("Розетки"), String(_ctrl.Type))) {
                _ctrl.Type = static_cast<CtrlType>(b.build().value().toInt32());
            }
            if (b.Button("ctrl_add"_h, F("Добавить"))) {
                if (_ctrl.Name == "") {
                    _ctrl.Error = F("Пустое имя");
                } else if (Controllers.isExists(_ctrl.Name)) {
                    _ctrl.Error = F("Имя занято");
                } else {
                    switch (_ctrl.Type) {
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

            if (b.beginGroup(F("Редактировать"))) {
                Controller *curCtrl = nullptr;
                String      sCtrls = "";
                size_t      last = Controllers.getControllers().size() - 1;
                size_t      i = 0;

                for (auto *ctrl : Controllers.getControllers()) {
                    sCtrls += ctrl->getName();
                    if (i < last) {
                        sCtrls += ";";
                    }
                    i++;
                }
                if (b.Select("ctrl_ed_sel"_h, F("Выбрать"), sCtrls, String(_ctrl.curCtrl))) {
                    _ctrl.curCtrl = b.build().value().toInt32();
                }
                if (Controllers.getControllers().size() > 0) {
                    curCtrl = Controllers.getControllers()[_ctrl.curCtrl];
                }
                if (b.Input("ctrl_ed_name"_h, F("Имя"), (curCtrl == nullptr) ? "" : curCtrl->getName())) {
                    if (b.build().value().toString() != "") {
                        if (curCtrl != nullptr) {
                            curCtrl->setName(b.build().value().toString());
                            b.reload();
                        }
                        _ctrl.Error = "";
                    } else {
                        _ctrl.Error = F("Пустое имя");
                    }
                }
                if (b.Button("ctrl_rm"_h, "Удалить", sets::Colors::Red)) {
                    if (Controllers.getControllers().size() > 0) {
                        Controllers.remove(_ctrl.curCtrl);
                        _ctrl.curCtrl = 0;
                        b.reload();
                    }
                }
                b.endGroup();
            }
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

void WebGUIClass::_updateCtrlsPage(sets::Updater& upd)
{
    Controller *curCtrl = nullptr;
    if (Controllers.getControllers().size() > 0) {
        curCtrl = Controllers.getControllers()[_ctrl.curCtrl];
        upd.update("ctrl_ed_sel"_h, String(_ctrl.curCtrl));
        upd.update("ctrl_ed_name"_h, curCtrl->getName());    
    }
}

void WebGUIClass::_buildSocketsPage(sets::Builder& b)
{
    auto *sock = static_cast<SocketCtrl *>(Controllers.getController(_ctrl.Name));

    if (b.beginGroup(F("Контроллер"))) {
        b.Label(F("Имя"), _ctrl.Name);
        b.Label(F("Тип"), F("Розетки"));
        if (_socket.Error != "") {
            b.Label(F("Ошибка"), _socket.Error, sets::Colors::Red);
        }
        if (b.Switch("ctrl_sock_en"_h, F("Включить"), String(sock->getEnabled()))) {
            sock->setEnabled(b.build().value().toBool());
        }
        b.endGroup();
    }

    if (b.beginMenu(F("Настройки"))) {
        if (b.beginGroup(F("Добавить Розетку"))) {
            if (b.Input("ctrl_sock_name"_h, "Имя", _socket.Name)) {
                _socket.Name = b.build().value().toString();
                if (_socket.Name == "") {
                    _socket.Error = F("Пустое имя");
                } else if (sock->isExists(_socket.Name)) {
                    _socket.Error = F("Имя занято");
                } else {
                    sock->addSocket(new Socket(_socket.Name));
                    _socket.Error = "";
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
            Socket                      *curSocket = nullptr;

            Interfaces.getInterfacesByType(outputs, IF_TYPE_RELAY);
            Interfaces.getInterfacesByType(inputs, IF_TYPE_DIGITAL_INPUT);

            for (size_t i = 0; i < sock->getSockets().size(); i++) {
                sSockets += sock->getSockets()[i]->getName();
                if (i < (sock->getSockets().size() - 1)) {
                    sSockets += ";";
                }
            }

            for (size_t i = 0; i < outputs.size(); i++) {
                sOut += outputs[i]->getName() + ";";
            }

            for (size_t i = 0; i < inputs.size(); i++) {
                sIn += inputs[i]->getName() + ";";
            }

            if (b.Select("ctrl_sock_sel"_h, F("Розетка"), sSockets, String(_socket.curSock))) {
                _socket.curSock = b.build().value().toInt32();

                if (sock->getSockets().size() > 0) {
                    Interfaces.getInterfacesByType(outputs, IF_TYPE_RELAY);
                    Interfaces.getInterfacesByType(inputs, IF_TYPE_DIGITAL_INPUT);

                    curSocket = sock->getSockets()[_socket.curSock];

                    if (curSocket->getInterface(SOCK_IF_RELAY) == nullptr) {
                        _socket.curRly = outputs.size();
                    } else {
                        for (size_t i = 0; i < outputs.size(); i++) {
                            if (outputs[i]->getName() == curSocket->getInterface(SOCK_IF_RELAY)->getName()) {
                                _socket.curRly = i;
                                break;
                            }
                        }
                    }

                    if (curSocket->getInterface(SOCK_IF_BUTTON) == nullptr) {
                        _socket.curBtn = inputs.size();
                    } else {
                        for (size_t i = 0; i < inputs.size(); i++) {
                            if (inputs[i]->getName() == curSocket->getInterface(SOCK_IF_BUTTON)->getName()) {
                                _socket.curBtn = i;
                                break;
                            }
                        }
                    }
                }
            }

            if (sock->getSockets().size() > 0) {
                curSocket = sock->getSockets()[_socket.curSock];
            }

            if (b.Input("ctrl_sock_ename"_h, F("Имя"), (curSocket != nullptr) ? curSocket->getName() : "")) {
                String name = b.build().value().toString();
                if (name == "") {
                    _socket.Error = F("Пустое имя");
                } else if (sock->isExists(name)) {
                    _socket.Error = F("Имя занято");
                } else {
                    if (sock->getSockets().size() > 0) {
                        curSocket->setName(name);
                        b.reload();
                    }
                }
            }

            if (b.Select("ctrl_sock_erly"_h, F("Реле"), sOut, String(_socket.curRly))) {
                _socket.curRly = b.build().value().toInt32();
                if (sock->getSockets().size() > 0) {
                    curSocket->setInterface(SOCK_IF_RELAY, (_socket.curRly == outputs.size()) ? nullptr : outputs[_socket.curRly]);
                }
            }

            if (b.Select("ctrl_sock_ebtn"_h, F("Кнопка"), sIn, String(_socket.curBtn))) {
                _socket.curBtn = b.build().value().toInt32();
                if (sock->getSockets().size() > 0) {
                    curSocket->setInterface(SOCK_IF_BUTTON, (_socket.curBtn == inputs.size()) ? nullptr : inputs[_socket.curBtn]);
                }
            }

            if (b.Button("ctrl_sock_rm"_h, F("Удалить"), sets::Colors::Red)) {
                if (sock->getSockets().size() > 0) {
                    sock->remove(_socket.curSock);
                    _ctrl.curCtrl = 0;
                    b.reload();
                }
            }
            b.endGroup();
        }
        b.endMenu();
    }

    if (b.beginGroup(F("Общее"))) {
        b.beginButtons();           
        if (b.Button("ctrl_sock_onall"_h, F("Включить все"))) {
            auto *sock = static_cast<SocketCtrl *>(Controllers.getController(_ctrl.Name));
            for (auto s : sock->getSockets()) {
                s->setStatus(true, true);
            }
            b.reload();
        }
        if (b.Button("ctrl_sock_offall"_h, F("Отключить все"), sets::Colors::Red)) {
            auto *sock = static_cast<SocketCtrl *>(Controllers.getController(_ctrl.Name));
            for (auto s : sock->getSockets()) {
                s->setStatus(false, true);
            }
            b.reload();
        }
        b.endButtons();
        b.endGroup();
    }

    if (b.beginGroup(F("Розетки"))) {
        for (auto s : sock->getSockets()) {
            if (b.Switch(su::SH(String("ctrl_sock_sw_" + s->getName()).c_str()), s->getName(), String(s->getStatus()))) {
                s->setStatus(b.build().value().toBool());
            }      
        }
        b.endGroup();
    }       
}

void WebGUIClass::_updateSocketsPage(sets::Updater& upd)
{
    auto *sock = static_cast<SocketCtrl *>(Controllers.getController(_ctrl.Name));
    Socket *curSocket = nullptr;

    upd.update("ctrl_sock_en"_h, String(sock->getEnabled()));

    if (sock->getSockets().size() > 0) {
        curSocket = sock->getSockets()[_socket.curSock];
    }

    upd.update("ctrl_sock_sel"_h, String(_socket.curSock));
    upd.update("ctrl_sock_erly"_h, String(_socket.curRly));
    upd.update("ctrl_sock_ebtn"_h, String(_socket.curBtn));
    upd.update("ctrl_sock_ename"_h, (curSocket != nullptr) ? curSocket->getName() : "");

    for (auto s : sock->getSockets()) {
        upd.update(su::SH(String("ctrl_sock_sw_" + s->getName()).c_str()), String(s->getStatus()));
    }
}

void WebGUIClass::_buildSettingsPage(sets::Builder& b)
{
    if (b.beginGroup(F("Настройки"))) {
        b.beginButtons();
        if (b.Button("set_sv"_h, F("Сохранить"))) {
            Configs.writeAll();
            _curPage = WEB_PAGE_MAIN;
            b.reload();
        }
        if (b.Button("set_rm"_h, F("Удалить"), sets::Colors::Red)) {
            Configs.eraseAll();
            _curPage = WEB_PAGE_MAIN;
            b.reload();
        }
        b.endButtons();
        b.endGroup();
    }

    if (b.beginGroup(F("Система"))) {
        b.beginButtons();
        if (b.Button("set_res"_h, F("Рестарт"))) {
            ESP.restart();
        }
        b.endButtons();
        b.endGroup();
    }
}

WebGUIClass WebGUI;
