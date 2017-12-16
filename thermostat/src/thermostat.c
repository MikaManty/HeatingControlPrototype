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
#include <string.h>
#include <mosquitto.h>
#include <mosquittoHelpersThermostat.h>
#include <jsonHelpersThermostat.h>
#include <thermostat.h>

/* Handle a message */
static void onMessageCb(struct mosquitto *m, void *context,
                       const struct mosquitto_message *msg)
{
	float temperature, averageTemperature;
	int sensorId, contextIndex, valveOpening;
	struct thermostatContextData *contextPtr;

	contextPtr = context;

	temperature = parseJSONSensorReport(msg->payload, &sensorId);

	/* check if sensor already exists */
	for(contextIndex = 0; contextIndex < MAX_NUMBER_OF_ACTIVE_SENSORS; contextIndex++)
	{
		if(contextPtr->sensors[contextIndex].sensorId == sensorId)
		{ /* sensor found. Update temperature */
			contextPtr->sensors[contextIndex].temperature = temperature;
			break;
		}
	}
	if(contextIndex == MAX_NUMBER_OF_ACTIVE_SENSORS)
	{ /* sensor not found. New sensor */
		for(contextIndex = 0; contextIndex < MAX_NUMBER_OF_ACTIVE_SENSORS; contextIndex++)
		{
			if(contextPtr->sensors[contextIndex].active == false)
			{
				contextPtr->sensors[contextIndex].active = true;
				contextPtr->sensors[contextIndex].sensorId = sensorId;
				contextPtr->sensors[contextIndex].temperature = temperature;
				contextPtr->numbnerOfActiveSensors++;
				break;
			}
		}
	}
	if(contextIndex == MAX_NUMBER_OF_ACTIVE_SENSORS)
	{
		printf("Maximum number of supported sensors reached. Message ignored");
	}

	/* bit of printf() spamming for output/testing */
	printf("NumOfSensors:%d TargetTemp:%.1f Sensors (index:temp)",contextPtr->numbnerOfActiveSensors,contextPtr->targetTemperature);
	for(contextIndex = 0; contextIndex < contextPtr->numbnerOfActiveSensors; contextIndex++)
	{
		printf("%d:%.1f ",contextIndex, contextPtr->sensors[contextIndex].temperature);
	}
	printf("\n");

	averageTemperature = calculateRoomAverageTemperature(contextPtr);
	//valveOpening = calculateRoomRadiatorValveOpening(contextPtr, averageTemperature);
}

int main(void) {
	int returnCode;
	struct mosquitto *mosqPtr;
	struct thermostatContextData context;

	/* intialize context data */
	memset(&context,0,sizeof(context));
	context.targetTemperature = 22.0; /* TODO: make configurable */

	mosqPtr = mosquittoInitAndCreate("thermostat", &context);
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

float calculateRoomAverageTemperature(struct thermostatContextData *contextPtr)
{
	int contextIndex;
	float sum=0;

	if(contextPtr->numbnerOfActiveSensors == 0)
		return 0;

	for(contextIndex = 0; contextIndex < MAX_NUMBER_OF_ACTIVE_SENSORS; contextIndex++)
	{
		if(contextPtr->sensors[contextIndex].active == true)
			sum = sum + contextPtr->sensors[contextIndex].temperature;
	}

	return sum/contextPtr->numbnerOfActiveSensors;
}

int calculateRoomRadiatorValveOpening(struct thermostatContextData *contextPtr, float currentAverageTemperature)
{
	float tempDifference;

	tempDifference = currentAverageTemperature - contextPtr->targetTemperature;
	if(abs(tempDifference) > FAST_ADJUSTMENT_THRESHOLD)
	{
		if(tempDifference > 0)
			return min((contextPtr->previousValveOpening - 10),0);
		else
			return max((contextPtr->previousValveOpening + 10),100);
	}
	else if((abs(tempDifference) > MEDIUM_ADJUSTMENT_THRESHOLD))
	{
		if(tempDifference > 0)
			return min((contextPtr->previousValveOpening - 5),0);
		else
			return max((contextPtr->previousValveOpening + 5),100);
	}
	else
	{ /* close to target. Only very minor adjustments */
		if(tempDifference > 0)
			return min((contextPtr->previousValveOpening - 1),0);
		else
			return max((contextPtr->previousValveOpening + 1),100);
	}
}
