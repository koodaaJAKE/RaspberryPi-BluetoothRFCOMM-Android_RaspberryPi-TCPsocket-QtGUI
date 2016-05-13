#include "TCP_SocketClient.h"

TCP_SocketClient::TCP_SocketClient()
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
    /* Create socket */
    if((m_sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("Could not create socket \n");
        return false;
    }

    memset(&m_server, 0, sizeof(m_server));
    m_server.sin_family = AF_INET;
    m_server.sin_port = htons(m_port);

    /* Convert std::string to a const char* */
    const char * c_address = m_address.c_str();

    /*This function converts the character string into a network
       address structure in the af address family, then copies the network
       address structure to dst.*/
    if(inet_pton(AF_INET, c_address, &m_server.sin_addr)<=0)
    {
        perror("inet_pton error occured\n");
        return false;
    }

    /* Connect */
    if( connect(m_sock_fd, (struct sockaddr *)&m_server, sizeof(m_server)) < 0)
    {
       perror("Connect Failed \n");
       return false;
    }

    return true;
}

bool TCP_SocketClient::SendAndReceiveSocketData(HRLVEZ0_Data_t *Data)
{
    memset(this->m_recvBuf, '0',sizeof(this->m_recvBuf));
    this->m_sendBuf[0] = 'S';

    /* Send character "S" to the socket to receive data */
    if(send(m_sock_fd, this->m_sendBuf, sizeof(this->m_sendBuf), 0) < 0)
    {
        perror("Send failed!\n");
        return false;
    }

    /* Receive the socket data */
    if(recv(m_sock_fd, this->m_recvBuf, sizeof(this->m_recvBuf), 0) < 0)
    {
        perror("Couldn't read data from the socket!\n");
        return false;
    }

    /* Deserialize the recieved data */
    Data->distance = DeserializeIntDistance(this->m_recvBuf);

    this->m_IntSpeed = DeserializeIntSpeed(this->m_recvBuf);

    Data->speed = Deserialize754_32(this->m_IntSpeed);

    return true;
}

bool TCP_SocketClient::SendConnectCommand(void)
{
    /* Send "C" to the socket to connect */
    this->m_sendBuf[0] = 'C';
    if(send(m_sock_fd, this->m_sendBuf, sizeof(this->m_sendBuf), 0) < 0)
    {
        perror("Send failed!\n");
        return false;
    }

    return true;
}

bool TCP_SocketClient::SendAlreadyConnectedCommand(void)
{
    /* Send "A" to send already connected command */
    this->m_sendBuf[0] = 'A';
    if(send(m_sock_fd, this->m_sendBuf, sizeof(this->m_sendBuf), 0) < 0)
    {
        perror("Send failed!\n");
        return false;
    }

    return true;
}

bool TCP_SocketClient::SendQuitCommand(void)
{
    /* Send "Q" to the socket to quit */
    this->m_sendBuf[0] = 'Q';
    if(send(m_sock_fd, this->m_sendBuf, sizeof(this->m_sendBuf), 0) < 0)
    {
        perror("Send failed!\n");
        return false;
    }

    return true;
}

bool TCP_SocketClient::SendClearLCD_Command(void)
{
    /* Send "L" to clear the LCD screen */
    this->m_sendBuf[0] = 'L';
    if(send(m_sock_fd, this->m_sendBuf, sizeof(this->m_sendBuf), 0) < 0)
    {
        perror("Send failed!\n");
        return false;
    }

    return true;
}

float TCP_SocketClient::Deserialize754Float(unsigned int floatspeed, unsigned int bits, unsigned int expbits)
{
    /* -1 for sign bit */
    this->m_significantbits = bits - expbits -1;

    if(floatspeed == 0)
        return 0.0;

    /*pull the significant*/
    this->m_result = (floatspeed & ((1<<this->m_significantbits)-1)); //mask
    this->m_result /= (1 << this->m_significantbits); //convert back to float
    this->m_result += 1.0f; //add the one back on

    //deal with the exponent
    this->m_bias = (1 << (expbits-1)) -1;
    this->m_shift = ((floatspeed >> this->m_significantbits) & ((1 << expbits)-1)) - this->m_bias;

    while(this->m_shift > 0)
    {
        this->m_result *= 2.0;
        this->m_shift--;
    }

    while(this->m_shift < 0)
    {
        this->m_result /= 2.0;
        this->m_shift++;
    }

    /*sign it*/
    this->m_result *= (floatspeed >> (bits-1)) & 1 ? -1.0 : 1.0;

    return this->m_result;
}

unsigned int TCP_SocketClient::DeserializeIntSpeed(unsigned char *buf)
{
    this->m_value = buf[4] << 24 | buf[5] << 16 | buf[6] << 8 | buf[7];

    return this->m_value;
}

unsigned int TCP_SocketClient::DeserializeIntDistance(unsigned char *buf)
{
    this->m_value = buf[0] << 24 | buf[1] << 16 | buf[2] << 8 | buf[3];

    return this->m_value;
}

unsigned int TCP_SocketClient::DeserializeIntPrevDistance(unsigned char *buf)
{
    this->m_value = buf[8] << 24 | buf[9] << 16 | buf[10] << 8 | buf[11];

    return this->m_value;
}
