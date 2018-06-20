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
#ifndef _UDPSERVER_WINDOW_H_
#define _UDPSERVER_WINDOW_H_

#include "../ControllerPatcherIncludes.hpp"

#define DEFAULT_UDP_PORT    8113

#define WIIU_CP_UDP_CONTROLLER_READ_DATA        0x03

class UDPServer {
    friend class ControllerPatcher;

private:
    static UDPServer *getInstance() {
        if(instance == NULL)
            instance = new UDPServer(DEFAULT_UDP_PORT);
        return instance;
    }

    static void destroyInstance() {
        if(instance != NULL) {
            delete instance;
            instance = NULL;
        }
    }

    UDPServer(int32_t port);
    ~UDPServer();
    void StartUDPThread(UDPServer * server);
    static void DoUDPThread(CThread *thread, void *arg);
    void DoUDPThreadInternal();
    BOOL cpyIncrementBufferOffset(void * target, void * source, int32_t * offset, int32_t typesize, int32_t maximum);

    volatile int32_t sockfd = -1;
    volatile int32_t exitThread = 0;

    static UDPServer *instance;
    static CThread *pThread;
};

#endif //_UDPSERVER_WINDOW_H_
