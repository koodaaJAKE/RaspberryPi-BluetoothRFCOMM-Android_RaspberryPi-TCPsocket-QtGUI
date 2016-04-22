/*
 * hrlvez0.h
 *
 *  Created on: 9.7.2015
 *      Author: root
 */

#ifndef HRLVEZ0_H_
#define HRLVEZ0_H_

#include <stdio.h>
#include <unistd.h> //Used for UART
#include <fcntl.h> //Used for UART
#include <termios.h> //Used for UART
#include <stdlib.h>
#include <string.h>
#include <errno.h>

/* Structure for the HRLVEZ0 raw distance data reading */
typedef struct data_frame
{
	unsigned char data1;
	unsigned char data2;
	unsigned char data3;
	unsigned char data4;
	unsigned char end;
}  __attribute__ ((packed)) data_frame_t;

/* Structure for actual data which is serialized and sent through TCP and Bluetooth RFCOMM sockets */
typedef struct HRLVEZ0_Data
{
	long distance;
	long prevdistance;
	float speed;
} HRLVEZ0_Data_t;

/* Function prototypes */
int setupHRLVEZ0_Serial(void);
int serialHRLVEZ0_Close(void);
int measureHRLVEZ0_Data(HRLVEZ0_Data_t *HRLVEZ0_Data);

#endif /* HRLVEZ0_H_ */
