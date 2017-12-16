/*
 * jsonHelper.h
 *
 *  Created on: Dec 16, 2017
 *      Author: mika
 */

#ifndef INC_JSONHELPERSTHERMOSTAT_H_
#define INC_JSONHELPERSTHERMOSTAT_H_

float parseJSONSensorReport(char * JSONStringPtr, int * sensorIdPtr /*out*/);
char * createJSONValveControl(int valveLevelPercentage);

#endif /* INC_JSONHELPERSTHERMOSTAT_H_ */
