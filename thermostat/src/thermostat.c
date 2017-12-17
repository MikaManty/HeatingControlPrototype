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
#include <unistd.h>
#include <mosquitto.h>
#include <mosquittoHelpersThermostat.h>
#include <jsonHelpersThermostat.h>
#include <thermostat.h>

void readMainArgumentsAndSetupBrokerAddress(int argc, char *argv[], char **ipAddress, int *portPtr, float *temperaturePtr)
{
	int option;

    while ( (option=getopt(argc, argv, "i:t:p:")) != EOF ) {
      switch ( option ) {
        case 'p': *portPtr = atoi(optarg);
                  break;
        case 't': *temperaturePtr = atof(optarg);
                  break;
        case 'i': *ipAddress = optarg;
                  break;
        case '?': fprintf(stderr,"Unknown option %c\n", optopt);
                  exit(1);
                  break;
      }
    }
    printf("Broker IP address: %s\n", *ipAddress);
    printf("Broker port: %d\n", *portPtr);
    printf("Temperature target: %f\n", *temperaturePtr);
}

/* Handle a message */
static void onMessageCb(struct mosquitto *m, void *context,
                       const struct mosquitto_message *msg)
{
	float temperature, averageTemperature;
	int sensorId, contextIndex, valveOpening,returnCode;
	struct thermostatContextData *contextPtr;
	char *jSONValveControlPtr;

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
	valveOpening = calculateRoomRadiatorValveOpening(contextPtr, averageTemperature);
	contextPtr->previousValveOpening = valveOpening;

	/* create and send valve opening message */
	jSONValveControlPtr = createJSONValveControl(valveOpening);

	returnCode = mosquitto_publish(m,NULL,"/actuators/room-1", /* only one room supported for now */
			strlen(jSONValveControlPtr),jSONValveControlPtr,1,false); /* QoS set to 1 to guarantee delivery. Duplicated do not matter */
	if (returnCode != MOSQ_ERR_SUCCESS) {
		printf("mosquitto_publish failed. errno:%d\n",errno);
		exit(EXIT_FAILURE);
	}

}

int main (int argc, char *argv[])
{
	int returnCode;
	struct mosquitto *mosqPtr;
	struct thermostatContextData context;
	float tempArgument;
    int BrokerPort = BROKER_DEFAULT_PORT;
    char *BrokerIpAddress = BROKER_DEFAULT_HOSTNAME;

	readMainArgumentsAndSetupBrokerAddress(argc, argv, &BrokerIpAddress, &BrokerPort, &tempArgument);

	/* intialize context data */
	memset(&context,0,sizeof(context));
	context.targetTemperature = tempArgument;

	mosqPtr = mosquittoInitAndCreate("thermostat", &context);
	mosquittoRegisterCallbacks(mosqPtr);
	mosquitto_message_callback_set(mosqPtr, onMessageCb); // register and define onMessageCb in this file to temporarily avoid circular dependency

	returnCode = mosquitto_connect(mosqPtr, BrokerIpAddress, BrokerPort, KEEPALIVE_SECONDS);
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

	return sum/(contextPtr->numbnerOfActiveSensors);
}

int calculateRoomRadiatorValveOpening(struct thermostatContextData *contextPtr, float currentAverageTemperature)
{
	float tempDifference;

	tempDifference = currentAverageTemperature - contextPtr->targetTemperature;
	if(abs(tempDifference) > FAST_ADJUSTMENT_THRESHOLD)
	{
		if(tempDifference > 0)
			return max((contextPtr->previousValveOpening - 10),0);
		else
			return min((contextPtr->previousValveOpening + 10),100);
	}
	else if((abs(tempDifference) > MEDIUM_ADJUSTMENT_THRESHOLD))
	{
		if(tempDifference > 0)
			return max((contextPtr->previousValveOpening - 5),0);
		else
			return min((contextPtr->previousValveOpening + 5),100);
	}
	else
	{ /* close to target. Only very minor adjustments */
		if(tempDifference > 0)
			return max((contextPtr->previousValveOpening - 1),0);
		else
			return min((contextPtr->previousValveOpening + 1),100);
	}
}
