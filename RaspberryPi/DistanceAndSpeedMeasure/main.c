/*
 * main.cpp
 */

#include "LCD.h"
#include "HRLVEZ0.h"
#include "thread.h"

#define TCPSOCKET

/* Global flag which terminates the thread while loop */
volatile sig_atomic_t thread_loop_flag = 0;

void sigHandler(int sig)
{
	printf("I got the signal..exiting\n!");
	//Set the flag to exit thread loops
	thread_loop_flag = 1;
}

int main(void)
{
	signal(SIGINT, sigHandler);

	/* Thread sensor data structure */
	thread_data_t *sensorData;
	sensorData = malloc(sizeof(thread_data_t));

#ifdef TCPSOCKET
	pthread_t measureHRLVEZ0Thread, TCP_SocketThread;
	int iret, iret1;
#else
	pthread_t measureHRLVEZ0Thread, bluetoothRFCOMMThread;
	int iret, iret2;
#endif

	setupHRLVEZ0_Serial();
	setupLCD_Serial();
	usleep(1000);

	clear_LCD();
	setType_LCD(4, 16);
	setBacklight_LCD(250);

	initializeMutex(sensorData);

	/************************************************************/
	/* Create Threads                                           */
	/************************************************************/
#ifdef TCPSOCKET
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
#else
	iret = pthread_create(&measureHRLVEZ0Thread, NULL, measureHRLVEZ0, (void*)sensorData);
	if(iret)
	{
		fprintf(stderr, "Error - pthread_create() return code: %d\n", iret);
		exit(EXIT_FAILURE);
	}

	iret2 = pthread_create(&bluetoothRFCOMMThread, NULL, bluetoothRFCOMM, (void*)sensorData);
	if(iret2)
	{
		fprintf(stderr, "Error - pthread_create() return code: %d\n", iret2);
		exit(EXIT_FAILURE);
	}
#endif

	/* Let the threads exit */
#ifdef TCPSOCKET
	pthread_join(measureHRLVEZ0Thread, NULL);
	pthread_join(TCP_SocketThread, NULL);
#else
	pthread_join(measureHRLVEZ0Thread, NULL);
	pthread_join(bluetoothRFCOMMThread, NULL);
#endif
	printf("Threads closed!\n");

	free(sensorData);
	clear_LCD();
	setBacklight_LCD(0);
    serialLCD_Close();
    serialHRLVEZ0_Close();
    return 0;
}



