/*
 * main.cpp
 */

#include "LCD.h"
#include "HRLVEZ0.h"
#include "thread.h"
#include "MYSQL.h"

#define TCPSOCKET

int main(void)
{
	/* Thread sensor data structure */
	thread_data_t sensorData;

	/* Initialize the structure of MySQL constants */
	mySqlConstants p_mySqlConstants = {
		.HOST = "localhost",
		.USER = "root",
		.PASSWORD = "*******",
		.DATABASE = "jakenDB"
	};

	/* Initialize the signal handler */
	signal(SIGINT, sigHandler);

#ifdef TCPSOCKET
	pthread_t measureHRLVEZ0Thread, TCP_SocketThread, mySqlThread;
	int iret, iret1, iret3;
#else
	pthread_t measureHRLVEZ0Thread, bluetoothRFCOMMThread, mySqlThread;
	int iret, iret2, iret3;
#endif

	setupHRLVEZ0_Serial();
	setupLCD_Serial();
	usleep(1000);

	clear_LCD();
	setType_LCD(4, 16);
	setBacklight_LCD(250);

	initializeMutex(&sensorData);

	initializeMySQL(&p_mySqlConstants);

	/************************************************************/
	/* Create Threads                                           */
	/************************************************************/
#ifdef TCPSOCKET
	iret = pthread_create(&measureHRLVEZ0Thread, NULL, measureHRLVEZ0, (void*)&sensorData);
	if(iret)
	{
		fprintf(stderr, "Error - pthread_create() return code: %d\n", iret);
		exit(EXIT_FAILURE);
	}

	iret1 = pthread_create(&TCP_SocketThread, NULL, TCP_Socket, (void*)&sensorData);
	if(iret1)
	{
		fprintf(stderr, "Error - pthread_create() return code: %d\n", iret1);
		exit(EXIT_FAILURE);
	}

	iret3 = pthread_create(&mySqlThread, NULL, mySQL, (void*)&sensorData);
	if(iret3)
	{
		fprintf(stderr, "Error - pthread_create() return code: %d\n", iret3);
		exit(EXIT_FAILURE);
	}
#else
	iret = pthread_create(&measureHRLVEZ0Thread, NULL, measureHRLVEZ0, (void*)&sensorData);
	if(iret)
	{
		fprintf(stderr, "Error - pthread_create() return code: %d\n", iret);
		exit(EXIT_FAILURE);
	}

	iret2 = pthread_create(&bluetoothRFCOMMThread, NULL, bluetoothRFCOMM, (void*)&sensorData);
	if(iret2)
	{
		fprintf(stderr, "Error - pthread_create() return code: %d\n", iret2);
		exit(EXIT_FAILURE);
	}

	iret3 = pthread_create(&mySqlThread, NULL, mySQL, (void*)&sensorData);
	if(iret3)
	{
		fprintf(stderr, "Error - pthread_create() return code: %d\n", iret3);
		exit(EXIT_FAILURE);
	}
#endif

	/* Let the threads exit */
#ifdef TCPSOCKET
	pthread_join(measureHRLVEZ0Thread, NULL);
	pthread_join(TCP_SocketThread, NULL);
	pthread_join(mySqlThread, NULL);
#else
	pthread_join(measureHRLVEZ0Thread, NULL);
	pthread_join(bluetoothRFCOMMThread, NULL);
	pthread_join(mySqlThread, NULL);
#endif
	printf("Threads closed!\n");
	clear_LCD();
	setBacklight_LCD(0);
    serialLCD_Close();
    serialHRLVEZ0_Close();
    return 0;
}



