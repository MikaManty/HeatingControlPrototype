/*
 * MosquittoHelpersRadiatorValve.h
 *
 *  Created on: Dec 16, 2017
 *      Author: mika
 */

#ifndef INC_MOSQUITTOHELPERSRADIATORVALVE_H_
#define INC_MOSQUITTOHELPERSRADIATORVALVE_H_

struct mosquitto* mosquittoInitAndCreate(char *sensorID, void *context);
void mosquittoRegisterCallbacks(struct mosquitto*);

#endif /* INC_MOSQUITTOHELPERSRADIATORVALVE_H_ */
