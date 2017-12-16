/*
 ============================================================================
 Name        : temperatureSensor.c
 Author      :
 Version     :
 Copyright   : Your copyright notice
 Description :
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <errno.h>
#include <mosquitto.h>

#define KEEPALIVE_SECONDS 60
#define BROKER_HOSTNAME "localhost"
#define BROKER_PORT 1883

static void on_connect(struct mosquitto *m, void *udata, int res) {
    if (res == 0)              /* success */
    	printf("mosquitto_connect success!\n");
    else
        EXIT_FAILURE;
}

/* A message was successfully published. */
static void on_publish(struct mosquitto *m, void *udata, int m_id) {
   	printf("mosquitto_publish success!\n");
}

/* Handle a message that just arrived via one of the subscriptions. */
static void on_message(struct mosquitto *m, void *udata,
                       const struct mosquitto_message *msg) {
   	printf("mosquitto message received\n");
}

/* Successful subscription hook. */
static void on_subscribe(struct mosquitto *m, void *udata, int mid,
                         int qos_count, const int *granted_qos) {
   	printf("mosquitto_subscribe success\n");
}

int main(void) {
	int major;
	int minor;
	int revision;
	int version;
	int returnCode;
	struct mosquitto *mosqPtr;
	int counter=0;

	mosquitto_lib_init();
	version = mosquitto_lib_version(&major,&minor,&revision);

	printf("Mosquitto Client version: %d \n",version);

	mosqPtr = mosquitto_new("Moskiitto1", true, NULL);
	if(mosqPtr == NULL)
	{
		printf("mosquitto_new failed. errno:%d\n",errno);
		EXIT_FAILURE;
	}

    mosquitto_connect_callback_set(mosqPtr, on_connect);
    mosquitto_publish_callback_set(mosqPtr, on_publish);
    mosquitto_subscribe_callback_set(mosqPtr, on_subscribe);
    mosquitto_message_callback_set(mosqPtr, on_message);

	returnCode = mosquitto_connect(mosqPtr, BROKER_HOSTNAME, BROKER_PORT, KEEPALIVE_SECONDS);
	if(returnCode != MOSQ_ERR_SUCCESS)
	{
		printf("mosquitto_connect failed. errno:%d\n",errno);
		EXIT_FAILURE;
	}

    size_t maxTopicSize = 32;
    char topic[maxTopicSize];
    snprintf(topic, maxTopicSize, "Version/%d", version);

	for(;;){
		usleep(1000);
		counter++;
	    size_t maxPayloadSize = 32;
	    char payload[maxPayloadSize];
	    size_t payloadLength = 0;
	    payloadLength = snprintf(payload, maxPayloadSize, "count: %d",counter);

		returnCode = mosquitto_publish(mosqPtr,NULL,topic,
				payloadLength, payload,0,false);
		if (returnCode != MOSQ_ERR_SUCCESS) {
			printf("mosquitto_publish failed. errno:%d\n",errno);
			EXIT_FAILURE;
		}
	}

	return EXIT_SUCCESS;
}
