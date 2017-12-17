/*
 * jsonHelpersRadiatorValve.c
 *
 *  Created on: Dec 16, 2017
 *      Author: mika
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <cJSON.h>
#include <jsonHelpersRadiatorValve.h>

int parseJSONValveControl(char * JSONStringPtr)
{
	cJSON *root;
	int level;

	root = cJSON_Parse(JSONStringPtr);
	cJSON *levelItem = cJSON_GetObjectItemCaseSensitive(root, "level");
	if (cJSON_IsNumber(levelItem))
	{
	  level = levelItem->valuedouble;
	}
	return level;
}
