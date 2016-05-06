/*
 * Bluetooth_RFCOMM.h
 */

#ifndef BLUETOOTH_RFCOMM_H_
#define BLUETOOTH_RFCOMM_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <bluetooth/sdp.h>
#include <bluetooth/sdp_lib.h>
#include <bluetooth/rfcomm.h>
#include <errno.h>
#include "thread.h"

typedef enum { false, true } bool;

#define NUMBER_OF_ADDRESSES 10
#define LENGTH_OF_BLTADDR	18
#define LENGTH_OF_BLTNAME	40

#define FRAME_END_CHAR				   0xEE

/* Bluetooth transfer messages */
#define BLUETOOTH_SOCKET_CLOSE_COMMAND 0x01
#define PRINT_HELLO					   0x02
#define READ_HRLVEZ0_DATA			   0x03
#define RANDOM_TEXT					   0x04
#define CLEAR_SCREEN				   0x05

/* Function prototypes */
int bluetoothRFCOMM_Client(thread_data_t *sensorData);
int bluetoothRFCOMM_Server(thread_data_t *sensorData);

#endif /* BLUETOOTH_RFCOMM_H_ */
