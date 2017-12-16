/*
 * mqttHelpers.h
 *
 *  Created on: Dec 16, 2017
 *      Author: mika
 */

#ifndef INC_MOSQUITTOHELPERS_H_
#define INC_MOSQUITTOHELPERS_H_

struct mosquitto* mosquittoInitAndCreate(char *sensorID);
void mosquittoRegisterCallbacks(struct mosquitto*);


/* Callback functions
void onConnectCb(struct mosquitto *m, void *udata, int res);
void onPublishCb(struct mosquitto *m, void *udata, int m_id);
void onMessageCb(struct mosquitto *m, void *udata,
						const struct mosquitto_message *msg);
void onSubscribeCb(struct mosquitto *m, void *udata, int mid,
                	int qos_count, const int *granted_qos);
*/

#endif /* INC_MOSQUITTOHELPERS_H_ */
