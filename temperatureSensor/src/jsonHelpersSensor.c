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
#include <jsonHelpersSensor.h>

/* Create and return temperature report payload in JSON format */
char * createJSONSensorReport(int sensorId,float temperature)
{
	cJSON *root;
	char buffer[100];
	int stringLength;

	/* Create sensor-Id string */
	stringLength = snprintf ( buffer, 100, "sensor-%d", sensorId);
	if (stringLength ==0 || stringLength>100) /* safety check */
	{
		printf("ERROR: snprintf() returned out of bounds value. Exiting");
		exit(EXIT_FAILURE);
	}

	root = cJSON_CreateObject();
	if (root == NULL)
	{
		printf("ERROR: cJSON_CreateObject() returned NULL. Exiting");
		exit(EXIT_FAILURE);
	}

	cJSON_AddItemToObject(root, "sensorID", cJSON_CreateString(buffer));
	cJSON_AddItemToObject(root, "type", cJSON_CreateString("temperature"));
	cJSON_AddNumberToObject(root,"value", temperature);
	return cJSON_Print(root);
}

