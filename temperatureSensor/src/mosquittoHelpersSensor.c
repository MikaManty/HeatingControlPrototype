/*
 * mqttHelpers.c
 *
 *  Created on: Dec 16, 2017
 *      Author: mika
 */

/*- Callback functions -*/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <mosquitto.h>

/* connect result */
static void onConnectCb(struct mosquitto *m, void *udata, int res) {
    if (res == 0)              /* success */
    	printf("mosquitto_connect success\n");
    else
    {
    	printf("mosquitto_connect failed\n");
        EXIT_FAILURE;
    }
}

/* A message was successfully published. */
static void onPublishCb(struct mosquitto *m, void *udata, int m_id) {
   	return;
}

/* Successful subscription hook. */
static void onSubscribeCb(struct mosquitto *m, void *udata, int mid,
                         int qos_count, const int *granted_qos) {
   	printf("mosquitto_subscribe success\n");
}

struct mosquitto* mosquittoInitAndCreate(char *sensorID, void *context)
{
	int major;
	int minor;
	int revision;
	int version;
	struct mosquitto *mosqPtr;

	mosquitto_lib_init();

	version = mosquitto_lib_version(&major,&minor,&revision);
	printf("Mosquitto Client version: %d \n",version);

	mosqPtr = mosquitto_new(sensorID, true, context);
	if(mosqPtr == NULL)
	{
		printf("mosquitto_new failed. errno:%d\n",errno);
		exit(EXIT_FAILURE);
	}
	return mosqPtr;
}

void mosquittoRegisterCallbacks(struct mosquitto* mosqPtr)
{
    mosquitto_connect_callback_set(mosqPtr, onConnectCb);
    mosquitto_publish_callback_set(mosqPtr, onPublishCb);
    mosquitto_subscribe_callback_set(mosqPtr, onSubscribeCb);
}
