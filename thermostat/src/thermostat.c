/*
 * thermostat.c
 *
 *  Created on: Dec 16, 2017
 *      Author: mika
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <errno.h>
#include <mosquitto.h>
#include <mosquittoHelpers.h>


#define KEEPALIVE_SECONDS 60
#define BROKER_HOSTNAME "localhost"
#define BROKER_PORT 1883

int main(void) {
	int returnCode;
	struct mosquitto *mosqPtr;

	mosqPtr = mosquittoInitAndCreate("thermostat");
	mosquittoRegisterCallbacks(mosqPtr);

	returnCode = mosquitto_connect(mosqPtr, BROKER_HOSTNAME, BROKER_PORT, KEEPALIVE_SECONDS);
	if(returnCode != MOSQ_ERR_SUCCESS)
	{
		printf("mosquitto_connect failed. errno:%d\n",errno);
		exit(EXIT_FAILURE);
	}

	mosquitto_subscribe(mosqPtr, NULL, "readings/temperature", 0);

	for(;;){
		returnCode = mosquitto_loop(mosqPtr, -1, 1);
		if(returnCode != MOSQ_ERR_SUCCESS)
		{
			printf("mosquitto_loop failed. errno:%d\n",errno);
			mosquitto_destroy(mosqPtr);
			break;
		}
	}

	return EXIT_SUCCESS;
}
