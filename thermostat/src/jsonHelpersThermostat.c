/*
 * json.c
 *
 *  Created on: Dec 16, 2017
 *      Author: mika
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <cJSON.h>
#include <jsonHelpersThermostat.h>

/* returns "temperature" value and parses sensorId INTEGER to the sensorId argument */
float parseJSONSensorReport(char * JSONStringPtr, int * sensorIdPtr /*out*/)
{
	cJSON *root;
	float temperature;
	char *charPositionPtr;

	root = cJSON_Parse(JSONStringPtr);

	cJSON *sensorIDItem = cJSON_GetObjectItemCaseSensitive(root, "sensorID");
	/* parse sensorId number from the sensorId string */
	charPositionPtr = sensorIDItem->valuestring;
	while (*charPositionPtr)
	{
	    if (isdigit(*charPositionPtr))
	    {
	    	*sensorIdPtr = strtol(charPositionPtr, &charPositionPtr, 10); // number found. read it.
	    }
	    else
	    {
	    	charPositionPtr++;
	    }
	}

	cJSON *valueItem = cJSON_GetObjectItemCaseSensitive(root, "value");
	if (cJSON_IsNumber(valueItem))
	{
	  temperature = valueItem->valuedouble;
	}
	return temperature;
}

