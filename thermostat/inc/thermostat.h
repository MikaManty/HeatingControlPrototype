/*
 * thermostat.h
 *
 *  Created on: Dec 16, 2017
 *      Author: mika
 */

#ifndef INC_THERMOSTAT_H_
#define INC_THERMOSTAT_H_

#define KEEPALIVE_SECONDS 60
#define BROKER_HOSTNAME "localhost"
#define BROKER_PORT 1883

#define MAX_NUMBER_OF_ACTIVE_SENSORS 12 /* up to "dozen" as requested */
#define FAST_ADJUSTMENT_THRESHOLD 10
#define MEDIUM_ADJUSTMENT_THRESHOLD 5

 #define max(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })

 #define min(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a < _b ? _a : _b; })

struct sensorContextData{
	bool active;
	int sensorId;
	float temperature;
};

struct thermostatContextData {
   int numbnerOfActiveSensors;
   float targetTemperature;
   int previousValveOpening;
   struct sensorContextData sensors[MAX_NUMBER_OF_ACTIVE_SENSORS];
};

float calculateRoomAverageTemperature(struct thermostatContextData *contextPtr);
int calculateRoomRadiatorValveOpening(struct thermostatContextData *contextPtr, float currentAverageTemperature);

#endif /* INC_THERMOSTAT_H_ */
