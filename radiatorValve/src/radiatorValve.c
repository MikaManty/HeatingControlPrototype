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
#include <mosquittoHelpersRadiatorValve.h>
#include <jsonHelpersRadiatorValve.h>


#define KEEPALIVE_SECONDS 60
#define BROKER_DEFAULT_HOSTNAME "localhost"
#define BROKER_DEFAULT_PORT 1883

void readMainArgumentsAndSetupBrokerAddress(int argc, char *argv[], char **ipAddress, int *portPtr)
{
	int option;

    while ( (option=getopt(argc, argv, "i:p:")) != EOF ) {
      switch ( option ) {
        case 'p': *portPtr = atoi(optarg);
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
}

/* Handle a message */
static void onMessageCb(struct mosquitto *m, void *context,
                       const struct mosquitto_message *msg)
{
	int radiatorValveOpening;

	radiatorValveOpening = parseJSONValveControl(msg->payload);

	printf("radiatorValve opening set to %d\n", radiatorValveOpening);
}

int main (int argc, char *argv[])
{
    int returnCode;
    int BrokerPort = BROKER_DEFAULT_PORT;
    char *BrokerIpAddress = BROKER_DEFAULT_HOSTNAME;
    struct mosquitto *mosqPtr;

    readMainArgumentsAndSetupBrokerAddress(argc, argv, &BrokerIpAddress, &BrokerPort);

	mosqPtr = mosquittoInitAndCreate("radiatorValve", NULL);
	mosquittoRegisterCallbacks(mosqPtr);
	mosquitto_message_callback_set(mosqPtr, onMessageCb); /* register and define onMessageCb in this file to temporarily avoid circular dependency */

	returnCode = mosquitto_connect(mosqPtr, BrokerIpAddress, BrokerPort, KEEPALIVE_SECONDS);
	if(returnCode != MOSQ_ERR_SUCCESS)
	{
		printf("mosquitto_connect failed. errno:%d\n",errno);
		exit(EXIT_FAILURE);
	}

	mosquitto_subscribe(mosqPtr, NULL, "/actuators/room-1", 1); /* QoS set to 1 to get at least one message each time*/

	for(;;)
	{
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
