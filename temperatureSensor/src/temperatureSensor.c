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
#define BROKER_HOSTNAME "localhost"
#define BROKER_PORT 1883

int main(void) {
	char sensorIDString[100];
	int returnCode;
	struct mosquitto *mosqPtr;
	char *jSONTempSensorReportPtr;
	int sensorId=6;
	float temperature=15.6;

	snprintf(sensorIDString, 100, "Sensor-%d",sensorId);

	mosqPtr = mosquittoInitAndCreate(sensorIDString);
	mosquittoRegisterCallbacks(mosqPtr);

	returnCode = mosquitto_connect(mosqPtr, BROKER_HOSTNAME, BROKER_PORT, KEEPALIVE_SECONDS);
	if(returnCode != MOSQ_ERR_SUCCESS)
	{
		printf("mosquitto_connect failed. errno:%d\n",errno);
		exit(EXIT_FAILURE);
	}

	for(;;){
		usleep(1000); /* publish every 1ms */

	    /*size_t maxPayloadSize = 32;
	    char payload[maxPayloadSize];
	    size_t payloadLength = 0;
	    payloadLength = snprintf(payload, maxPayloadSize, "count: %d",6);
*/
		jSONTempSensorReportPtr = createJSONSensorReport(sensorId,temperature);

		returnCode = mosquitto_publish(mosqPtr,NULL,"readings/temperature",
				strlen(jSONTempSensorReportPtr),jSONTempSensorReportPtr,0,false);
		if (returnCode != MOSQ_ERR_SUCCESS) {
			printf("mosquitto_publish failed. errno:%d\n",errno);
			exit(EXIT_FAILURE);
		}
	}

	return EXIT_SUCCESS;
}
