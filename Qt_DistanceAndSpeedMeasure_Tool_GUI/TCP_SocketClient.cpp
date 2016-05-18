#include "TCP_SocketClient.h"

TCP_SocketClient::TCP_SocketClient() :
    m_sock_fd ( 0 ),
    m_recvBuf { 0 },
    m_sendBuf { 0 },
    m_deserializedInt ( 0 )
{
    m_port = PORT_NUMBER;
    m_address = ADDRESS;
}

TCP_SocketClient::~TCP_SocketClient()
{
    close(m_sock_fd);
}

bool TCP_SocketClient::CreateSocket()
{
    struct sockaddr_in client;
    /* Create socket */
    if((m_sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("Could not create socket \n");
        return false;
    }

    memset(&client, 0, sizeof(client));
    client.sin_family = AF_INET;
    client.sin_port = htons(m_port);

    /* Convert std::string to a const char* */
    const char *c_address = m_address.c_str();

    /*This function converts the character string into a network
       address structure in the af address family, then copies the network
       address structure to dst.*/
    if(inet_pton(AF_INET, c_address, &client.sin_addr)<=0)
    {
        perror("inet_pton error occured\n");
        return false;
    }

    /* Connect */
    if(connect(m_sock_fd, (struct sockaddr *)&client, sizeof(client)) < 0)
    {
       perror("Connect Failed \n");
       return false;
    }

    return true;
}

bool TCP_SocketClient::SendAndReceiveSocketData(HRLVEZ0_Data_t *Data)
{
    memset(m_recvBuf, '0',sizeof(m_recvBuf));
    m_sendBuf[0] = 'S';

    /* Send character "S" to the socket to receive data */
    if(send(m_sock_fd, m_sendBuf, sizeof(m_sendBuf), 0) < 0)
    {
        perror("Send failed!\n");
        return false;
    }

    /* Receive the socket data */
    if(recv(m_sock_fd, m_recvBuf, sizeof(m_recvBuf), 0) < 0)
    {
        perror("Couldn't read data from the socket!\n");
        return false;
    }

    /* Deserialize the recieved data */
    Data->distance = DeserializeIntDistance(m_recvBuf);

    unsigned int intSpeed = DeserializeIntSpeed(m_recvBuf);

    Data->speed = Deserialize754_32(intSpeed);

    return true;
}

bool TCP_SocketClient::SendConnectCommand(void)
{
    /* Send "C" to the socket to connect */
    m_sendBuf[0] = 'C';
    if(send(m_sock_fd, m_sendBuf, sizeof(m_sendBuf), 0) < 0)
    {
        perror("Send failed!\n");
        return false;
    }

    return true;
}

bool TCP_SocketClient::SendAlreadyConnectedCommand(void)
{
    /* Send "A" to send already connected command */
    m_sendBuf[0] = 'A';
    if(send(m_sock_fd, m_sendBuf, sizeof(m_sendBuf), 0) < 0)
    {
        perror("Send failed!\n");
        return false;
    }

    return true;
}

bool TCP_SocketClient::SendQuitCommand(void)
{
    /* Send "Q" to the socket to quit */
    m_sendBuf[0] = 'Q';
    if(send(m_sock_fd, m_sendBuf, sizeof(m_sendBuf), 0) < 0)
    {
        perror("Send failed!\n");
        return false;
    }

    return true;
}

bool TCP_SocketClient::SendClearLCD_Command(void)
{
    /* Send "L" to clear the LCD screen */
    m_sendBuf[0] = 'L';
    if(send(m_sock_fd, m_sendBuf, sizeof(m_sendBuf), 0) < 0)
    {
        perror("Send failed!\n");
        return false;
    }

    return true;
}

float TCP_SocketClient::Deserialize754Float(unsigned int floatspeed, unsigned int bits, unsigned int expbits)
{
    float result;
    int shift;
    unsigned int bias;
    unsigned int significantbits;

    /* -1 for sign bit */
    significantbits = bits - expbits -1;

    if(floatspeed == 0)
        return 0.0;

    /*pull the significant*/
    result = (floatspeed & ((1 << significantbits)-1)); //mask
    result /= (1 << significantbits); //convert back to float
    result += 1.0f; //add the one back on

    //deal with the exponent
    bias = (1 << (expbits-1)) -1;
    shift = ((floatspeed >> significantbits) & ((1 << expbits)-1)) - bias;

    while(shift > 0)
    {
        result *= 2.0;
        shift--;
    }

    while(shift < 0)
    {
        result /= 2.0;
        shift++;
    }

    /*sign it*/
    result *= (floatspeed >> (bits-1)) & 1 ? -1.0 : 1.0;

    return result;
}

unsigned int TCP_SocketClient::DeserializeIntSpeed(unsigned char *buf)
{
    m_deserializedInt = buf[4] << 24 | buf[5] << 16 | buf[6] << 8 | buf[7];

    return m_deserializedInt;
}

unsigned int TCP_SocketClient::DeserializeIntDistance(unsigned char *buf)
{
    m_deserializedInt = buf[0] << 24 | buf[1] << 16 | buf[2] << 8 | buf[3];

    return m_deserializedInt;
}

unsigned int TCP_SocketClient::DeserializeIntPrevDistance(unsigned char *buf)
{
    m_deserializedInt = buf[8] << 24 | buf[9] << 16 | buf[10] << 8 | buf[11];

    return m_deserializedInt;
}
