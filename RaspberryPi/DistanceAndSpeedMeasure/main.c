/*
 * main.cpp
 *
 *  This software utilize MaxBotix HRLV-EZ0 ultrasonic sensor to calculate velocity of moving objects.
 *  Reading from the sensor is made with serial communication. Speed is printed to a LCD screen by serial communication.
 *
 */

#include "LCD.h"
#include "HRLVEZ0.h"
#include <signal.h>
#include "TCP_Socket.h"
#include "Bluetooth_RFCOMM.h"

void sigHandler(int sig)
{
	printf("I got the signal..exiting\n!");
    serialLCD_Close();
    serialHRLVEZ0_Close();
    exit(0);
}

int main(void)
{
	signal(SIGINT, sigHandler);

	setupHRLVEZ0_Serial();
	setupLCD_Serial();
	usleep(1000);

	clear_LCD();
	setType_LCD(4, 16);
	setBacklight_LCD(250);

	int chooseConnectionMethod = 0;

	printf("\n*********************************************\n");
	printf("Choose what Bluetooth connect method to use    \n");
	printf("1. for Server and 2. for Client connection       ");
	printf("\n*********************************************\n");

	scanf("%d", &chooseConnectionMethod);

	if(chooseConnectionMethod == 1) {
		bluetoothRFCOMM_Server();
	}
	else
		bluetoothRFCOMM_Client();

    //TCP_SocketPollingServer();

	clear_LCD();
	setBacklight_LCD(0);
    serialLCD_Close();
    serialHRLVEZ0_Close();
    return 0;
}



