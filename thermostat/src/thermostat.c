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
#include <mosquittoHelpersThermostat.h>
#include <jsonHelpersThermostat.h>

#define KEEPALIVE_SECONDS 60
#define BROKER_HOSTNAME "localhost"
#define BROKER_PORT 1883

/* Handle a message */
static void onMessageCb(struct mosquitto *m, void *udata,
                       const struct mosquitto_message *msg)
{
	float temperature;
	int sensorId;
	printf("got message '%.*s' for topic '%s'\n", msg->payloadlen, (char*) msg->payload, msg->topic);

	temperature = parseJSONSensorReport(msg->payload, &sensorId);
	printf("value from JSON temp= %f sensor =%d\n",temperature,sensorId);
}

int main(void) {
	int returnCode;
	struct mosquitto *mosqPtr;

	mosqPtr = mosquittoInitAndCreate("thermostat");
	mosquittoRegisterCallbacks(mosqPtr);

	mosquitto_message_callback_set(mosqPtr, onMessageCb); // register and define onMessageCb in this file to temporarily avoid circular dependency

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
