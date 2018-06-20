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
#include "UDPServer.hpp"
#include <malloc.h>
#include <stdio.h>
#include <string.h>

#include <utils/logger.h>

#define MAX_UDP_SIZE 0x578

CThread * UDPServer::pThread = NULL;
UDPServer * UDPServer::instance = NULL;

UDPServer::UDPServer(int32_t port) {
    int32_t ret;
    struct sockaddr_in addr;

    addr.sin_family = AF_INET;
    addr.sin_port = port;
    addr.sin_addr.s_addr = 0;

    this->sockfd = ret = socket(AF_INET, SOCK_DGRAM, 0);
    if(ret == -1) return;
    int32_t enable = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable));
    ret = bind(sockfd, (sockaddr *)&addr, 16);
    if(ret < 0) return;

    StartUDPThread(this);
}

UDPServer::~UDPServer() {
    CThread * pThreadPointer = UDPServer::pThread;
    if(pThreadPointer != NULL) {
        exitThread = 1;
        if(pThreadPointer != NULL) {
            delete pThreadPointer;
            UDPServer::pThread = NULL;
            if (this->sockfd != -1) {
                socketclose(sockfd);
            }
            this->sockfd = -1;
        }
    }
    if(HID_DEBUG) {
        DEBUG_FUNCTION_LINE("Thread has been closed\n");
    }


}

void UDPServer::StartUDPThread(UDPServer * server) {
    int32_t priority = 28;
    if(OSGetTitleID() == 0x00050000101c9300 || //The Legend of Zelda Breath of the Wild JPN
            OSGetTitleID() == 0x00050000101c9400 || //The Legend of Zelda Breath of the Wild USA
            OSGetTitleID() == 0x00050000101c9500 || //The Legend of Zelda Breath of the Wild EUR
            OSGetTitleID() == 0x00050000101c9b00 || //The Binding of Isaac: Rebirth EUR
            OSGetTitleID() == 0x00050000101a3c00) { //The Binding of Isaac: Rebirth USA
        priority = 10;
        DEBUG_FUNCTION_LINE("This game needs higher thread priority. We set it to %d\n",priority);
    }
    UDPServer::pThread = CThread::create(UDPServer::DoUDPThread, (void*)server, CThread::eAttributeAffCore2,priority);
    UDPServer::pThread->resumeThread();
}

BOOL UDPServer::cpyIncrementBufferOffset(void * target, void * source, int32_t * offset, int32_t typesize, int32_t maximum) {
    if(((int)*offset + typesize) > maximum) {
        DEBUG_FUNCTION_LINE("Transfer error. Excepted %04X bytes, but only got %04X\n",(*offset + typesize),maximum);
        return false;
    }
    memcpy(target,(void*)((uint32_t)source+(*offset)),typesize);
    *offset += typesize;
    return true;
}

void UDPServer::DoUDPThread(CThread *thread, void *arg) {
    UDPServer * args = (UDPServer * )arg;
    args->DoUDPThreadInternal();
}

void UDPServer::DoUDPThreadInternal() {
    uint8_t buffer[MAX_UDP_SIZE];
    int32_t n;

    my_cb_user  user;
    while(1) {
        //int32_t usingVar = exitThread;
        if(exitThread)break;
        memset(buffer,0,MAX_UDP_SIZE);
        n = recv(sockfd,buffer,MAX_UDP_SIZE,0);
        if (n < 0) {
            int32_t errno_ = socketlasterr();
            OSSleepTicks(OSMicrosecondsToTicks(2000));
            if(errno_ != 11 && errno_ != 9) {
                break;
            }
            continue;
        }
        int32_t bufferoffset = 0;
        uint8_t type;
        memcpy((void *)&type,buffer,sizeof(type));
        bufferoffset += sizeof(type);
        switch (buffer[0]) {
        case WIIU_CP_UDP_CONTROLLER_READ_DATA: {
            if(gUsedProtocolVersion >= WIIU_CP_TCP_HANDSHAKE_VERSION_1) {
                uint8_t count_commands;
                memcpy((void *)&count_commands,buffer+bufferoffset,sizeof(count_commands));
                bufferoffset += sizeof(count_commands);
                for(int32_t i = 0; i<count_commands; i++) {
                    int32_t handle;
                    uint16_t deviceSlot;
                    uint32_t hid;
                    uint8_t padslot;
                    uint8_t datasize;

                    if(!cpyIncrementBufferOffset((void *)&handle,       (void *)buffer,&bufferoffset,sizeof(handle),    n))continue;
                    if(!cpyIncrementBufferOffset((void *)&deviceSlot,   (void *)buffer,&bufferoffset,sizeof(deviceSlot),n))continue;
                    hid = (1  << deviceSlot);
                    if(!cpyIncrementBufferOffset((void *)&padslot,      (void *)buffer,&bufferoffset,sizeof(padslot),   n))continue;
                    if(!cpyIncrementBufferOffset((void *)&datasize,     (void *)buffer,&bufferoffset,sizeof(datasize),  n))continue;
                    uint8_t * databuffer = (uint8_t*) malloc(datasize * sizeof(uint8_t));
                    if(!databuffer) {
                        DEBUG_FUNCTION_LINE("Allocating memory failed\n");
                        continue;
                    }

                    if(!cpyIncrementBufferOffset((void *)databuffer,    (void *)buffer,&bufferoffset,datasize,          n))continue;
                    //DEBUG_FUNCTION_LINE("UDPServer::DoUDPThreadInternal(): Got handle: %d slot %04X hid %04X pad %02X datasize %02X\n",handle,deviceSlot,hid,padslot,datasize);

                    user.pad_slot = padslot;
                    user.slotdata.deviceslot =  deviceSlot;
                    user.slotdata.hidmask = hid;

                    if(gNetworkController[deviceSlot][padslot][0] == 0) {
                        DEBUG_FUNCTION_LINE("Ehm. Pad is not connected. STOP SENDING DATA ;) \n");
                    } else {
                        ControllerPatcherHID::externHIDReadCallback(handle,databuffer,datasize,&user);
                    }
                    if(databuffer) {
                        free(databuffer);
                        databuffer = NULL;
                    }
                }
                break;
            }
            break;
        }
        default: {
            break;
        }
        }
    }
    if(HID_DEBUG) {
        DEBUG_FUNCTION_LINE("UDPServer Thread ended\n");
    }
}
