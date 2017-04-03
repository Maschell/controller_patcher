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
#include "TCPServer.hpp"
#include <malloc.h>
#include <stdio.h>
#include <sys/time.h>
#include <string.h>
#include "../ControllerPatcher.hpp"
#include "./ControllerPatcherNet.hpp"

#define WIIU_CP_TCP_HANDSHAKE       0x12
#define WIIU_CP_TCP_SAME_CLIENT     0x20
#define WIIU_CP_TCP_NEW_CLIENT      0x21

#define ATTACH 0x01
#define DETACH 0x00

#define WIIU_CP_TCP_ATTACH      0x01
#define WIIU_CP_TCP_DETACH      0x02
#define WIIU_CP_TCP_PING        0xF0
#define WIIU_CP_TCP_PONG        0xF1

#define WIIU_CP_TCP_ATTACH_CONFIG_FOUND         0xE0
#define WIIU_CP_TCP_ATTACH_CONFIG_NOT_FOUND     0xE1
#define WIIU_CP_TCP_ATTACH_USER_DATA_OKAY       0xE8
#define WIIU_CP_TCP_ATTACH_USER_DATA_BAD        0xE9

#define errno (*__gh_errno_ptr())

ControllerPatcherThread * TCPServer::pThread = NULL;
TCPServer * TCPServer::instance = NULL;

TCPServer::TCPServer(int port){
    this->sockfd = -1;
    this->clientfd = -1;
    memset(&(this->sock_addr),0,sizeof(this->sock_addr));
    TCPServer::AttachDetach(DETACH);
	StartTCPThread(this);
}

TCPServer::~TCPServer(){
    CloseSockets();
    if(HID_DEBUG) log_printf("TCPServer: Thread will be closed\n");
    TCPServer::AttachDetach(DETACH);
    exitThread = 1;
    if(TCPServer::pThread != NULL){
        if(HID_DEBUG) log_printf("TCPServer: Deleting it!\n");
        delete TCPServer::pThread;
    }
    if(HID_DEBUG) log_printf("TCPServer: Thread done\n");
    TCPServer::pThread = NULL;
}

void TCPServer::CloseSockets(){
    if (this->sockfd != -1){
        socketclose(this->sockfd);
    }
    if (this->clientfd != -1){
        socketclose(this->clientfd);
    }
    this->sockfd = -1;
    this->clientfd = -1;
}

void TCPServer::StartTCPThread(TCPServer * server){
    TCPServer::pThread = ControllerPatcherThread::create(TCPServer::DoTCPThread, (void*)server, ControllerPatcherThread::eAttributeAffCore2,28);
    TCPServer::pThread->resumeThread();
}

void TCPServer::AttachDetach(int attach){
    if(HID_DEBUG){
        if(attach){
            log_printf("TCPServer: Network Attach\n");
        }else{
            log_printf("TCPServer: Network Detach\n");
        }
    }

    for(int i= 0;i< gHIDMaxDevices;i++){
        for(int j= 0;j< HID_MAX_PADS_COUNT;j++){
            if(gNetworkController[i][j][NETWORK_CONTROLLER_ACTIVE] > 0){
                log_printf("Found a registered pad in deviceslot %d and padslot %d! Lets detach it.\n",i,j);
                HIDDevice device;
                memset(&device,0,sizeof(device));

                device.interface_index = 0;
                device.vid = gNetworkController[i][j][NETWORK_CONTROLLER_VID];
                device.pid = gNetworkController[i][j][NETWORK_CONTROLLER_PID];
                device.handle = gNetworkController[i][j][NETWORK_CONTROLLER_HANDLE];
                device.max_packet_size_rx = 8;
                ControllerPatcherHID::externAttachDetachCallback(&device,attach);
                memset(gNetworkController[i][j],0,sizeof(gNetworkController[i][j]));
            }
        }
    }

    if(HID_DEBUG){
        if(attach){
            log_printf("TCPServer: Network Attach DONE!\n");
        }else{
            log_printf("TCPServer: Network Detach DONE!\n");
        }
    }
}

void TCPServer::DetachAndDelete(){
    TCPServer::AttachDetach(DETACH);
    memset(&gNetworkController,0,sizeof(gNetworkController));
}

int TCPServer::RunTCP(){
    int ret;
	while (1) {
        if(exitThread) break;
		ret = ControllerPatcherNet::checkbyte(clientfd);
		if (ret < 0) {
            if(errno != 6) return ret;
            usleep(1000);
			continue;
		}
        //log_printf("got byte from tcp! %01X\n",ret);
		switch (ret) {
            case WIIU_CP_TCP_ATTACH: { /*attach */
                int handle;
                ret = ControllerPatcherNet::recvwait(clientfd, &handle, 4);
                if(ret < 0){
                    log_printf("TCPServer::RunTCP() Error in %02X: recvwait handle\n",WIIU_CP_TCP_ATTACH);
                    return ret;
                }
                if(HID_DEBUG) log_printf("TCPServer: got handle %d\n",handle);
                u16 vid = 0;
                u16 pid = 0;
                ret = ControllerPatcherNet::recvwait(clientfd, &vid, 2);
                if(ret < 0){
                    log_printf("TCPServer::RunTCP() Error in %02X: recvwait vid\n",WIIU_CP_TCP_ATTACH);
                    return ret;
                }
               if(HID_DEBUG) log_printf("TCPServer: got vid %04X\n",vid);

                ret = ControllerPatcherNet::recvwait(clientfd, &pid, 2);
                if(ret < 0){
                    log_printf("TCPServer::RunTCP() Error in %02X: recvwait pid\n",WIIU_CP_TCP_ATTACH);
                    return ret;
                }
                if(HID_DEBUG) log_printf("TCPServer: got pid %04X\n",pid);
                HIDDevice device;
                memset(&device,0,sizeof(device));
                device.handle = handle;
                device.interface_index = 0;
                device.vid = SWAP16(vid);
                device.pid = SWAP16(pid);
                device.max_packet_size_rx = 8;

                my_cb_user * user  = NULL;
                ControllerPatcherHID::externAttachDetachCallback(&device,1);
                if((ret = ControllerPatcherUtils::getDataByHandle(handle,&user)) < 0){
                    log_printf("TCPServer::RunTCP() Error in %02X: getDataByHandle(%d,%08X).\n",WIIU_CP_TCP_ATTACH,handle,&user);
                    log_printf("TCPServer::RunTCP() Error in %02X: Config for the controller is missing.\n",WIIU_CP_TCP_ATTACH);
                    if((ret = ControllerPatcherNet::sendbyte(clientfd, WIIU_CP_TCP_ATTACH_CONFIG_NOT_FOUND) < 0)){
                        log_printf("TCPServer::RunTCP() Error in %02X: Sending the WIIU_CP_TCP_ATTACH_CONFIG_NOT_FOUND byte failed. Error: %d.\n",WIIU_CP_TCP_ATTACH,ret);
                    }
                    return -1;
                }
                if((ret = ControllerPatcherNet::sendbyte(clientfd, WIIU_CP_TCP_ATTACH_CONFIG_FOUND) < 0)){
                    log_printf("TCPServer::RunTCP() Error in %02X: Sending the WIIU_CP_TCP_ATTACH_CONFIG_FOUND byte failed. Error: %d.\n",WIIU_CP_TCP_ATTACH,ret);
                    return ret;
                }
                if(user != NULL){
                    if((ret = ControllerPatcherNet::sendbyte(clientfd, WIIU_CP_TCP_ATTACH_USER_DATA_OKAY) < 0)){
                        log_printf("TCPServer::RunTCP() Error in %02X: Sending the WIIU_CP_TCP_ATTACH_USER_DATA_OKAY byte failed. Error: %d.\n",WIIU_CP_TCP_ATTACH,ret);
                        return ret;
                    }

                    ret = ControllerPatcherNet::sendwait(clientfd,&user->slotdata.deviceslot,2);
                    if(ret < 0){
                        log_printf("TCPServer::RunTCP() Error in %02X: sendwait slotdata: %04X\n",WIIU_CP_TCP_ATTACH,user->slotdata.deviceslot);
                        return ret;
                    }
                    ret = ControllerPatcherNet::sendwait(clientfd,&user->pad_slot,1);
                    if(ret < 0){
                        log_printf("TCPServer::RunTCP() Error in %02X: sendwait pad_slot: %04X\n",WIIU_CP_TCP_ATTACH,user->pad_slot);
                        return ret;
                    }
                }else{
                    log_printf("TCPServer::RunTCP() Error in %02X: invalid user data.\n",WIIU_CP_TCP_ATTACH);
                    if((ret = ControllerPatcherNet::sendbyte(clientfd, WIIU_CP_TCP_ATTACH_USER_DATA_BAD) < 0)){
                        log_printf("TCPServer::RunTCP() Error in %02X: Sending the WIIU_CP_TCP_ATTACH_USER_DATA_BAD byte failed. Error: %d.\n",WIIU_CP_TCP_ATTACH,ret);
                        return ret;
                    }
                    return -1;
                    break;
                }

                if(HID_DEBUG) log_printf("TCPServer: attachted to device slot: %d , pad slot is: %d\n",user->slotdata.deviceslot,user->pad_slot);

                gNetworkController[user->slotdata.deviceslot][user->pad_slot][NETWORK_CONTROLLER_VID] = device.vid;
                gNetworkController[user->slotdata.deviceslot][user->pad_slot][NETWORK_CONTROLLER_PID] = device.pid;
                gNetworkController[user->slotdata.deviceslot][user->pad_slot][NETWORK_CONTROLLER_ACTIVE] = 1;
                gNetworkController[user->slotdata.deviceslot][user->pad_slot][NETWORK_CONTROLLER_HANDLE] = handle;

                if(HID_DEBUG) log_printf("TCPServer: handle %d connected! vid: %02X pid: %02X deviceslot %d, padslot %d\n",handle,vid,pid,user->slotdata.deviceslot,user->pad_slot);
                break;
            }
            case WIIU_CP_TCP_DETACH: { /*detach */
                int handle;
                ret = ControllerPatcherNet::recvwait(clientfd, &handle, 4);
                if(ret < 0){
                    log_printf("TCPServer::RunTCP() Error in %02X: recvwait handle\n",WIIU_CP_TCP_DETACH);
                    return ret;
                    break;
                }

                if(HID_DEBUG) log_printf("TCPServer: got detach for handle: %d\n",handle);
                my_cb_user * user  = NULL;
                if(ControllerPatcherUtils::getDataByHandle(handle,&user) < 0){
                    log_printf("TCPServer::RunTCP() Error in %02X: getDataByHandle(%d,%08X).\n",WIIU_CP_TCP_DETACH,handle,&user);
                    return -1;
                    break;
                }
                if(user == NULL){
                    log_printf("TCPServer::RunTCP() Error in %02X: invalid user data.\n",WIIU_CP_TCP_DETACH);
                    return -1;
                    break;
                }
                int deviceslot = user->slotdata.deviceslot;
                if(HID_DEBUG) log_printf("TCPServer: device slot is: %d , pad slot is: %d\n",deviceslot,user->pad_slot);

                DeviceVIDPIDInfo vidpid;
                int result;
                if((result = ControllerPatcherUtils::getVIDPIDbyDeviceSlot(deviceslot,&vidpid)) < 0){
                    log_printf("TCPServer::RunTCP() Error in %02X: Couldn't find a valid VID/PID for device slot %d. Error: %d\n",WIIU_CP_TCP_DETACH,deviceslot,ret);
                    return -1;
                    break;
                }

                HIDDevice device;
                memset(&device,0,sizeof(device));
                device.handle = handle;
                device.interface_index = 0;
                device.vid = SWAP16(vidpid.vid);
                device.pid = SWAP16(vidpid.pid);
                device.max_packet_size_rx = 14;

                ControllerPatcherHID::externAttachDetachCallback(&device,DETACH);
                memset(gNetworkController[deviceslot][user->pad_slot],0,sizeof(gNetworkController[deviceslot][user->pad_slot]));
                if(HID_DEBUG) log_printf("TCPServer: handle %d disconnected!\n",handle);
                break;
            }
            case WIIU_CP_TCP_PING: { /*ping*/
                if(HID_DEBUG) log_printf("TCPServer: GOT PING\n");
                int ret = ControllerPatcherNet::sendbyte(clientfd, WIIU_CP_TCP_PONG);
                if(ret < 0){ log_printf("TCPServer::RunTCP() Error in %02X: sendbyte PONG\n"); return -1;}

                break;
            }
            default:
                return -1;
                break;
		}
	}
	return 0;
}

void TCPServer::ErrorHandling(){
    CloseSockets();
    usleep(1000*1000*2);
}

void TCPServer::DoTCPThreadInternal(){
    int ret;
    s32 len;
    while (1) {
        if(exitThread) break;
        memset(&(this->sock_addr),0,sizeof(sock_addr));
		sock_addr.sin_family = AF_INET;
		sock_addr.sin_port = DEFAULT_TCP_PORT;
		sock_addr.sin_addr.s_addr = 0;

		this->sockfd = ret = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if(ret == -1){ ErrorHandling(); continue;}
        int enable = 1;

        setsockopt(this->sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable));

		ret = bind(this->sockfd, (sockaddr *)&sock_addr, 16);
		if(ret < 0) { ErrorHandling(); continue;}
		ret = listen(this->sockfd, 1);
		if(ret < 0){ ErrorHandling(); continue;}

        do{
            if(HID_DEBUG) log_printf("TCPServer::DoTCPThreadInternal: Waiting for a connection\n");
            if(exitThread) break;
            len = 16;

            clientfd = ret = accept(sockfd, (sockaddr *)&(sock_addr), &len);

            if(HID_DEBUG) log_printf("TCPServer::DoTCPThreadInternal: Connection accepted\n");
            if(ret == -1){ ErrorHandling(); break;}
            int ret;
            ret = ControllerPatcherNet::sendbyte(clientfd, WIIU_CP_TCP_HANDSHAKE); //Hey I'm a WiiU console!
            if(ret < 0){ log_printf("TCPServer::DoTCPThreadInternal: Error sendbyte: %02X\n",WIIU_CP_TCP_HANDSHAKE); ErrorHandling(); break;}


            if(ret < 0){ log_printf("TCPServer::DoTCPThreadInternal: Error sendbyte %02X/02X\n",WIIU_CP_TCP_NEW_CLIENT,WIIU_CP_TCP_SAME_CLIENT); ErrorHandling(); break;}
            TCPServer::DetachAndDelete(); //Clear connected controller
            RunTCP();

            if(clientfd != -1){
                socketclose(clientfd);
            }
            clientfd = -1;
        }while(0);
        TCPServer::DetachAndDelete(); //Clear connected controller
        CloseSockets();
        if(HID_DEBUG) log_printf("TCPServer: Connection closed\n");
		continue;
	}
}

void TCPServer::DoTCPThread(ControllerPatcherThread *thread, void *arg){
    TCPServer * args = (TCPServer * )arg;
    return args->DoTCPThreadInternal();
}
