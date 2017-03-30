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
#ifndef _TCPSERVER_WINDOW_H_
#define _TCPSERVER_WINDOW_H_

#include "../utils/ControllerPatcherThread.hpp"
#include "dynamic_libs/socket_functions.h"
#include "utils/logger.h"

#define DEFAULT_TCP_PORT    8112

class TCPServer{
    friend class ControllerPatcher;

private:
     static TCPServer *getInstance() {
        if(!instance)
            instance = new TCPServer(DEFAULT_TCP_PORT);
        return instance;
    }

    static void destroyInstance() {
        if(instance){
            delete instance;
            instance = NULL;
        }
    }

    TCPServer(int port);
    ~TCPServer();

    void CloseSockets();
    void ErrorHandling();

    void StartTCPThread(TCPServer * server);
    static void DoTCPThread(ControllerPatcherThread *thread, void *arg);
    void DoTCPThreadInternal();
    static void DetachConnectedNetworkController();
    static void AttachDetach(int attach);
    void DetachAndDelete();
    static TCPServer *instance;

    int RunTCP();

    struct sockaddr_in sock_addr;
    volatile int sockfd = -1;
    volatile int clientfd = -1;


    volatile int exitThread = 0;
    static ControllerPatcherThread *pThread;
};

#endif //_TCPSERVER_WINDOW_H_
