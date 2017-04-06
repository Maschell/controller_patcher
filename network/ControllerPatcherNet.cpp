#include "dynamic_libs/os_functions.h"
#include "dynamic_libs/socket_functions.h"
#include "ControllerPatcherNet.hpp"

int ControllerPatcherNet::recvwait(int sock, void *buffer, int len) {
	int ret;
	while (len > 0) {
		ret = recv(sock, buffer, len, 0);
		if(ret < 0) return ret;
		len -= ret;
		buffer =  (void *)(((char *) buffer) + ret);
	}
	return 0;
}

int ControllerPatcherNet::recvbyte(int sock) {
	unsigned char buffer[1];
	int ret;

	ret = recvwait(sock, buffer, 1);
	if (ret < 0) return ret;
	return buffer[0];
}

int ControllerPatcherNet::checkbyte(int sock) {
	unsigned char buffer[1];
	int ret;

	ret = recv(sock, buffer, 1, MSG_DONTWAIT);
	if (ret < 0) return ret;
	if (ret == 0) return -1;
	return buffer[0];
}

int ControllerPatcherNet::sendwait(int sock, const void *buffer, int len) {
	int ret;
	while (len > 0) {
		ret = send(sock, buffer, len, 0);
		if(ret < 0) return ret;
		len -= ret;
		buffer =  (void *)(((char *) buffer) + ret);
	}
	return 0;
}

int ControllerPatcherNet::sendbyte(int sock, unsigned char byte) {
	unsigned char buffer[1];
	buffer[0] = byte;
	return sendwait(sock, buffer, 1);
}
