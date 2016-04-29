/*
 * thread.c
 */
#include "HRLVEZ0.h"
#include "thread.h"
#include "TCP_Socket.h"
#include "TCP_Socket.h"

int initializeMutex(thread_data_t *init_mutex_t)
{
	int res;

	res = pthread_mutex_init(&init_mutex_t->mutex, NULL);
	if (res != 0) {
		perror("Mutex1 initialization failed \n");
		return -1;
	}
	return 0;
}

/* Thread for measuring the HRLVEZ0 ultra sonic sensor */
void *measureHRLVEZ0(void *arg)
{
	thread_data_t *sensorData = (thread_data_t*)arg;

	while(!thread_loop_flag)
	{
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
	while(!thread_loop_flag)
	{
		thread_data_t *sensorData = (thread_data_t*)arg;
		if(TCP_SocketPollingServer(sensorData) <= 0);
			thread_loop_flag = 1;

	}
	pthread_exit(NULL);
}
