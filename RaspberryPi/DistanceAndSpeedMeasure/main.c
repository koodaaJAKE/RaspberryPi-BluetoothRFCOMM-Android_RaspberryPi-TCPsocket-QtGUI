/*
 * main.cpp
 */

#include "LCD.h"
#include "HRLVEZ0.h"
#include <signal.h>
#include "Bluetooth_RFCOMM.h"
#include "thread.h"

/* Flag which terminates the thread while loop */
volatile sig_atomic_t thread_loop_flag = 0;

void sigHandler(int sig)
{
	printf("I got the signal..exiting\n!");

	//Set the boolean flag to exit thread loops
	thread_loop_flag = 1;
}

int main(void)
{
	signal(SIGINT, sigHandler);

	/* Thread sensor data structure */
	thread_data_t *sensorData;
	sensorData = malloc(sizeof(thread_data_t));

	pthread_t measureHRLVEZ0Thread, TCP_SocketThread;
	int iret, iret1;

	setupHRLVEZ0_Serial();
	setupLCD_Serial();
	usleep(1000);

	clear_LCD();
	setType_LCD(4, 16);
	setBacklight_LCD(250);
/*
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
*/

	initializeMutex(sensorData);

	/************************************************************/
	/* Create Threads                                           */
	/************************************************************/
	iret = pthread_create(&measureHRLVEZ0Thread, NULL, measureHRLVEZ0, (void*)sensorData);
	if(iret)
	{
		fprintf(stderr, "Error - pthread_create() return code: %d\n", iret);
		exit(EXIT_FAILURE);
	}

	iret1 = pthread_create(&TCP_SocketThread, NULL, TCP_Socket, (void*)sensorData);
	if(iret1)
	{
		fprintf(stderr, "Error - pthread_create() return code: %d\n", iret1);
		exit(EXIT_FAILURE);
	}

	/* Let the threads exit */
	pthread_join(measureHRLVEZ0Thread, NULL);
	pthread_join(TCP_SocketThread, NULL);

	free(sensorData);
	clear_LCD();
	setBacklight_LCD(0);
    serialLCD_Close();
    serialHRLVEZ0_Close();
    return 0;
}



