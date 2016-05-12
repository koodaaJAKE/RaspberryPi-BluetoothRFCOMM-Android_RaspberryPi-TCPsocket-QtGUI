/*
 * TCP_Socket.h
 */

#ifndef TCP_SOCKET_H_
#define TCP_SOCKET_H_

#include <sys/poll.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <errno.h>
#include "HRLVEZ0.h"
#include "thread.h"

#define CLIENT_ADDRESS "10.42.0.2"
#define PORT_NUMBER 51000

#define TRUE	1
#define FALSE	0

#define BUFFERSIZE	20

/* Function prototypes */
int TCP_SocketPollingServer(thread_data_t *sensorData);

#endif /* TCP_SOCKET_H_ */
