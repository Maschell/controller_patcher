/****************************************************************************
 * Copyright (C) 2016,2017 Maschell
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ****************************************************************************/
#ifndef _CP_TCPSERVER_WINDOW_H_
#define _CP_TCPSERVER_WINDOW_H_

#include "../ControllerPatcherIncludes.hpp"

#include <utils/TCPServer.hpp>
#include <network/net.h>
#include <coreinit/title.h>

#define WIIU_CP_TCP_HANDSHAKE               WIIU_CP_TCP_HANDSHAKE_VERSION_3

#define WIIU_CP_TCP_HANDSHAKE_VERSION_MIN   WIIU_CP_TCP_HANDSHAKE_VERSION_1
#define WIIU_CP_TCP_HANDSHAKE_VERSION_MAX   WIIU_CP_TCP_HANDSHAKE_VERSION_3

#define WIIU_CP_TCP_HANDSHAKE_VERSION_1     0x12
#define WIIU_CP_TCP_HANDSHAKE_VERSION_2     0x13
#define WIIU_CP_TCP_HANDSHAKE_VERSION_3     0x14

#define  WIIU_CP_TCP_HANDSHAKE_ABORT        0x30

#define WIIU_CP_TCP_ATTACH      0x01
#define WIIU_CP_TCP_DETACH      0x02
#define WIIU_CP_TCP_PING        0xF0
#define WIIU_CP_TCP_PONG        0xF1

#define WIIU_CP_TCP_ATTACH_CONFIG_FOUND         0xE0
#define WIIU_CP_TCP_ATTACH_CONFIG_NOT_FOUND     0xE1
#define WIIU_CP_TCP_ATTACH_USER_DATA_OKAY       0xE8
#define WIIU_CP_TCP_ATTACH_USER_DATA_BAD        0xE9

#define DEFAULT_TCP_PORT    8112

class CPTCPServer: TCPServer{
    friend class ControllerPatcher;

private:
     static CPTCPServer *getInstance() {
        if(!instance){

            instance = new CPTCPServer(DEFAULT_TCP_PORT);
        }
        return instance;
    }

    static void destroyInstance() {
        if(instance){
            delete instance;
            instance = NULL;
        }
    }

    static s32 getPriority(){
        s32 priority = 28;
        if(OSGetTitleID() == 0x00050000101c9300 || //The Legend of Zelda Breath of the Wild JPN
           OSGetTitleID() == 0x00050000101c9400 || //The Legend of Zelda Breath of the Wild USA
           OSGetTitleID() == 0x00050000101c9500 || //The Legend of Zelda Breath of the Wild EUR
           OSGetTitleID() == 0x00050000101c9b00 || //The Binding of Isaac: Rebirth EUR
           OSGetTitleID() == 0x00050000101a3c00){  //The Binding of Isaac: Rebirth USA
            priority = 10;
            DEBUG_FUNCTION_LINE("This game needs higher thread priority. We set it to %d\n",priority);
        }
        return priority;
    }

    CPTCPServer(s32 port);

    virtual ~CPTCPServer();

    static void DetachConnectedNetworkController();

    static void AttachDetach(HIDAttachEvent attach);

    virtual void DetachAndDelete();

    virtual bool whileLoop();

    virtual bool acceptConnection();

    virtual void onConnectionClosed();

    static CPTCPServer * instance;

};

#endif //_TCPSERVER_WINDOW_H_
