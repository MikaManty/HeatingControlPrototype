/*
 * mqttHelpers.h
 *
 *  Created on: Dec 16, 2017
 *      Author: mika
 */

#ifndef INC_MOSQUITTOHELPERSTHERMOSTAT_H_
#define INC_MOSQUITTOHELPERSTHERMOSTAT_H_

struct mosquitto* mosquittoInitAndCreate(char *sensorID, void *context);
void mosquittoRegisterCallbacks(struct mosquitto*);

#endif /* INC_MOSQUITTOHELPERSTHERMOSTAT_H_ */
