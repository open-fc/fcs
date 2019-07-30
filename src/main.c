/*
 ============================================================================
 Name        : main.c
 Author      : jiajiefeng
 Version     : 1.0
 Copyright   : OpenFC
 Description : Fuel Cell Control System(FCCS)
 ============================================================================
 */
#include "head.h"


int main(int argc, char *argv[])
{


	logger("Starting FCCS ...\n");
	init();

	if (argc == 2 && (strcmp(argv[1], "-m") == 0))
	{
		monitor();
	}

	pthread_t ctrl_t, disc_t,hb_t;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	pthread_attr_setschedpolicy(&attr, SCHED_FIFO);

	pthread_create(&ctrl_t, &attr, (void *) control, NULL);
	pthread_create(&disc_t, &attr, (void *) discharge, NULL);
	pthread_create(&hb_t, &attr, (void *) heartbeat, NULL);

	logger("FCCS started\n");
	while (1)
	{
		warn();
	}

	return EXIT_SUCCESS;
}

