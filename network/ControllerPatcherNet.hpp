#ifndef _CONTROLLERPATCHERNET_H_
#define _CONTROLLERPATCHERNET_H_

class ControllerPatcherNet{
    friend class TCPServer;
    friend class UDPServer;
    private:
        static int recvwait(int sock, void *buffer, int len);
        static int recvbyte(int sock);
        static int checkbyte(int sock);
        static int sendwait(int sock, const void *buffer, int len);
        static int sendbyte(int sock, unsigned char byte);
};

#endif
