#ifndef TCP_SOCKETCLIENT_H
#define TCP_SOCKETCLIENT_H

#include <iostream>
#include <cstdio>
#include <cstring>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>

/* Macro for deserializing float */
#define Deserialize754_32(i) (Deserialize754Float((i), 32, 8))

#define ADDRESS "10.42.0.2"
#define PORT_NUMBER 51000

#define BUFFER_SIZE 12

/* Structure for the HRLVEZ0 sensor data coming through TCP socket */
typedef struct HRLVEZ0_Data
{
    int distance;
    int prevdistance;
    float speed;
}HRLVEZ0_Data_t;

class TCP_SocketClient
{

public:

    TCP_SocketClient();
    ~TCP_SocketClient();
    bool CreateSocket();
    bool SendAndReceiveSocketData(HRLVEZ0_Data_t *Data);
    bool SendConnectCommand(void);
    bool SendAlreadyConnectedCommand(void);
    bool SendQuitCommand(void);
    bool SendClearLCD_Command(void);


private:

    float Deserialize754Float(unsigned int floatspeed, unsigned int bits, unsigned int expbits);
    unsigned int DeserializeIntSpeed(unsigned char *buf);
    unsigned int DeserializeIntDistance(unsigned char *buf);
    unsigned int DeserializeIntPrevDistance(unsigned char *buf);

    int m_sock_fd;
    std::string m_address;
    int m_port;

    unsigned char m_recvBuf[BUFFER_SIZE];
    unsigned char m_sendBuf[1];
    unsigned int m_deserializedInt;
};

#endif // TCP_SOCKETCLIENT_H
