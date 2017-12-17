/*
 * temperatureSensor.c
 *
 *  Created on: Dec 16, 2017
 *      Author: mika
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <mosquitto.h>
#include <cJSON.h>
#include <jsonHelpersSensor.h>
#include <mosquittoHelpersSensor.h>


#define KEEPALIVE_SECONDS 60
#define BROKER_DEFAULT_HOSTNAME "localhost"
#define BROKER_DEFAULT_PORT 1883
#define VALVE_OPENING_NOT_KNOWN 101

void readMainArgumentsAndSetupBrokerAddress(int argc, char *argv[], char **ipAddress, int *portPtr, int *sensorIdPtr, float *temperaturePtr)
{
	int option;

    while ( (option=getopt(argc, argv, "i:s:t:p:")) != EOF ) {
      switch ( option ) {
        case 'p': *portPtr = atoi(optarg);
                  break;
        case 't': *temperaturePtr = atof(optarg);
                  break;
        case 's': *sensorIdPtr = atoi(optarg);
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
    printf("Sensor Id: %d\n", *sensorIdPtr);
    printf("Start Temperature: %f\n", *temperaturePtr);
}

/* handle test message which changes the valve opening */
static void onMessageCb(struct mosquitto *m, void *context,
                       const struct mosquitto_message *msg)
{
	int *valveOpeningPtr;
	int newValveOpening;
	valveOpeningPtr = context;
	newValveOpening = atoi(msg->payload);
	*valveOpeningPtr = newValveOpening;
}

int dummyTemperatureAdjust(float temperature, int valveOpening)
{
	float newTemperature,valveOpeningTemperature;
	float difference;
	/* Temperature adjusted towards valveOpening is not at it.
	 * Temperature range 0-50 degrees. Linearly proportional
	 * to valve opening. Extremely dummy :)
	 *
	 * Speed of reduction proportional to the difference
	 * Maximum reduction hence 10 degrees at once per evaluation if e.g 0 degrees and valveOpening 100.
	 * Minimum reduction is only 0.1 degrees if difference is at minimum
	 *
	 * valveOpeningTemperature is where temperature saturates if opening kept stable
	 *
	 * 0.5 degree accuracy is where adjustment is stopped
	 * */

	valveOpeningTemperature = valveOpening/2;
	if(temperature > valveOpeningTemperature)
	{
		/* Need to reduce the temperature */
		difference = temperature - valveOpeningTemperature;
		if(difference > 0.5)
		{
			newTemperature = temperature - difference/10;
			printf("Temperature reduced. New value:%f ValveOpening:%d \n",newTemperature,valveOpening);
		}
		return newTemperature;
	}
	else if (temperature < valveOpeningTemperature)
	{
		/* Need to increase the temperature */
		difference = valveOpeningTemperature - temperature;
		if(difference > 0.5)
		{
			newTemperature = temperature + difference/10;
			printf("Temperature increased. New value:%f ValveOpening:%d \n",newTemperature,valveOpening);
		}
		return newTemperature;
	}
	/* otherwise no need to adjust */
	return temperature;
}

int main (int argc, char *argv[])
{
	char sensorIDString[100];
	int returnCode;
	struct mosquitto *mosqPtr;
	char *jSONTempSensorReportPtr;
	int sensorId=0, valveOpening=VALVE_OPENING_NOT_KNOWN;
	float temperature=15.6;
    int BrokerPort = BROKER_DEFAULT_PORT;
    char *BrokerIpAddress = BROKER_DEFAULT_HOSTNAME;

    readMainArgumentsAndSetupBrokerAddress(argc, argv, &BrokerIpAddress, &BrokerPort, &sensorId, &temperature);

	snprintf(sensorIDString, 100, "Sensor-%d",sensorId);

	mosqPtr = mosquittoInitAndCreate(sensorIDString, &valveOpening);
	mosquittoRegisterCallbacks(mosqPtr);
    mosquitto_message_callback_set(mosqPtr, onMessageCb);

	returnCode = mosquitto_connect(mosqPtr, BrokerIpAddress, BrokerPort, KEEPALIVE_SECONDS);
	if(returnCode != MOSQ_ERR_SUCCESS)
	{
		printf("mosquitto_connect failed. errno:%d\n",errno);
		mosquitto_destroy(mosqPtr);
		exit(EXIT_FAILURE);
	}

	/* subscribe test message which control temperature */
	mosquitto_subscribe(mosqPtr, NULL, "testing/valveOpening", 1);

	/* threaded reception of messages */
	mosquitto_loop_start(mosqPtr);

	for(;;){
		usleep(10000);/* Adjust and publish temperature periodically */
		if(valveOpening != VALVE_OPENING_NOT_KNOWN)
			temperature = dummyTemperatureAdjust(temperature, valveOpening);

		jSONTempSensorReportPtr = createJSONSensorReport(sensorId,temperature);

		returnCode = mosquitto_publish(mosqPtr,NULL,"readings/temperature",
				strlen(jSONTempSensorReportPtr),jSONTempSensorReportPtr,0,false);
		if (returnCode != MOSQ_ERR_SUCCESS) {
			printf("mosquitto_publish failed. errno:%d\n",errno);
			exit(EXIT_FAILURE);
		}
		usleep(10000);
	}

	return EXIT_SUCCESS;
}
