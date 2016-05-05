/*
 * thread.h
 */

#ifndef THREAD_H_
#define THREAD_H_

#include <pthread.h>
#include <stdio.h>
#include <signal.h>
#include "HRLVEZ0.h"

//Flag to terminate the thread loop
extern volatile sig_atomic_t thread_loop_flag;

typedef struct thread_data
{
	long distance;
	long prevDistance;
	float speed;
	pthread_mutex_t mutex;

}thread_data_t;

/* Function prototypes */
int initializeMutex(thread_data_t *init_mutex_t);
void *measureHRLVEZ0(void *arg);
void *TCP_Socket(void *arg);
void *bluetoothRFCOMM(void *arg);


#endif /* THREAD_H_ */
