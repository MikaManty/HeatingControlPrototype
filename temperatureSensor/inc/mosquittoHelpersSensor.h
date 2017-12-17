/*
 * mqttHelpers.h
 *
 *  Created on: Dec 16, 2017
 *      Author: mika
 */

#ifndef INC_MOSQUITTOHELPERSSENSOR_H_
#define INC_MOSQUITTOHELPERSSENSOR_H_

struct mosquitto* mosquittoInitAndCreate(char *sensorID, void *context);
void mosquittoRegisterCallbacks(struct mosquitto*);

#endif /* INC_MOSQUITTOHELPERSSENSOR_H_ */
