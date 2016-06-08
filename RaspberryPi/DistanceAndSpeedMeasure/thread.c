/*
 * thread.c
 */
#include "HRLVEZ0.h"
#include "thread.h"
#include "TCP_Socket.h"
#include "Bluetooth_RFCOMM.h"
#include "MYSQL.h"

/* Local flag for terminate the thread loops */
static volatile sig_atomic_t thread_loop_flag;

void sigHandler(int sig)
{
	printf("I got the signal..exiting\n!");
	//Set the flag to exit thread loops
	thread_loop_flag = 1;
}

int initializeMutex(thread_data_t *init_mutex_t)
{
	int res;

	res = pthread_mutex_init(&init_mutex_t->mutex, NULL);
	if(res != 0) {
		perror("Mutex1 initialization failed \n");
		return -1;
	}
	return 0;
}

/* Thread for measuring the HRLVEZ0 ultra sonic sensor */
void *measureHRLVEZ0(void *arg)
{
	thread_data_t *sensorData = (thread_data_t*)arg;

	while(!thread_loop_flag) {
		pthread_mutex_lock(&sensorData->mutex);
		measureHRLVEZ0_Data((HRLVEZ0_Data_t*)sensorData);
		pthread_mutex_unlock(&sensorData->mutex);
	}
	pthread_exit(NULL);
	pthread_mutex_destroy(&sensorData->mutex);
}

/* Thread for TCP Socket Server communication */
void *TCP_Socket(void *arg)
{
	while(!thread_loop_flag) {
		thread_data_t *sensorData = (thread_data_t*)arg;
		if(TCP_SocketPollingServer(sensorData) <= 0);
			thread_loop_flag = 1;
	}
	pthread_exit(NULL);
}

void *bluetoothRFCOMM(void *arg)
{
	while(!thread_loop_flag) {
		thread_data_t *sensorData = (thread_data_t*)arg;

		int chooseConnectionMethod = 0;

		printf("\n*********************************************\n");
		printf("Choose what Bluetooth connect method to use    \n");
		printf("1. for Server and 2. for Client connection       ");
		printf("\n*********************************************\n");

		scanf("%d", &chooseConnectionMethod);

		if(chooseConnectionMethod == 1) {
			if(bluetoothRFCOMM_Server(sensorData) <= 0)
				thread_loop_flag = 1;
		}
		else
			if(bluetoothRFCOMM_Client(sensorData) <= 0)
				thread_loop_flag = 1;
	}
	pthread_exit(NULL);
}

void *mySQL(void *arg)
{
	int id = 0;

	while(!thread_loop_flag) {
		thread_data_t *sensorData = (thread_data_t*)arg;
		insertDataToTable(id, sensorData);
		id++;
	}
	pthread_exit(NULL);
}
