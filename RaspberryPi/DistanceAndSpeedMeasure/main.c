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
    close(g_fd);
    close (uart0_filestream);
    exit(0);
}

int main(void)
{
	signal(SIGINT, sigHandler);

	setupHRLVEZ0_Serial();
	setupLCD_Serial();
	sleep(1);

	clear_LCD();
	usleep(200);

	setType_LCD(4, 16);
	usleep(200);

	setBacklight_LCD(250);
	usleep(200);
/*
	char strng[] = "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA";
	size_t l = strlen(strng);
	printLongString_LCD(strng, l);
	sleep(1);
*/

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
	usleep(200);
    close(g_fd);
    close (uart0_filestream);
    return 0;
}



