# HeatingControlPrototype
Simple prototype of heating control using MQTT communication and JSON format payloads.

## High level description
System consists of 3 separate applications described in below

### thermostat
This application is responsible of receiving temperature data of the room from 1-12 temperature sensors.

Messages are received by subscribing "readings/temperature" topic and payload format understood is JSON like below
{
	"sensorID":	"sensor-6",
	"type":	"temperature",
	"value":	26
}
 
New sensor is detected when message is received with unrecognized sensorID value. Thermostat has 12 item context array for book keeping and searches if sensor is already know.
This array contains also other book keeping information like target temperature and previously set valve opening. Array structures are shown below:

struct thermostatContextData {
   int numberOfActiveSensors;
   float targetTemperature;
   int previousValveOpening;
   struct sensorContextData sensors[MAX_NUMBER_OF_ACTIVE_SENSORS];

struct sensorContextData{
	bool active;
	int sensorId;
	float temperature;
};

Thermostat periodically calculates room temperature by averaging temperature values over all connected sensors.

Based on average temperature it publishes value opening value (0-100 %) to topic "/actuators/room-1" in JSON format like below:
{
	"level":	56
}
 
Adjustment algorithm itself is silly simple. If average temperature is lower than target then valve is opened a bit and if higher it is then closed a bit. Depending how large the difference is different amount of correction is done. There is two steps/thresholds for correction defined in thermostat.h. So as default reaction is larger is 
 #define FAST_ADJUSTMENT_THRESHOLD 5  <- Difference larger than this means larger correction    
 #define MEDIUM_ADJUSTMENT_THRESHOLD 0.1 <- Difference larger than this mens smaller correction and smaller no correction at all.
 
Note: Support for sensor removal is missing. Sensor can be dynamically added anytime though. Removal is TODO and should be added by using MQTT WILL. This would let thermostat to know that sensor is lost.

Running and arguments:
thermostat -i <IP of broker> -p <port of broker> -t <target temperature (float)>

### temperatureSensor
This application is responsible for publishing the temperature data for the thermostat periodically. Topic and format was described above under ##thermostat

Temperature change caused by by changing valve opening (heater either heating or letting room to cool) is mocked by subscribing testing MQTT topic. Could have used also "/actuators/room-1" topic messages from thermostat for this but instead but added one new topic for just testing purposes. It is at least external to requirements and can be easily removed from final application with real sensors. Subscription for test messages is to topic "/testing/valveOpening". No JSON is used for this messages. ValveOpening is only thing in the payload (as integer in ASCII). Below is one example from MQTT trace:

testing/valveOpening 56 

Temperature change algorithm itself is simple. It calculates temperature difference of current temperature and temperature which is assumed with current valve opening (=valve opening / 2). If there is difference then temperature is moved towards the temperature which valve opening should eventually cause. Temperature is always moved half way between current and the target. So heater is assumed super powerful if current temperature is far from the targeted.

Running and arguments:
temperatureSensor -i <IP of broker> -p <port of broker> -s <sensorID 0-MAX(int)> -t <starting temperature>

## radiatorValve
This application is responsible of subscribing "/actuators/room-1" topic. RadiatorValve is parsing JSON format payload and the publishes obtained valve opening "level" using /testing/valveOpening" topic as describes above under temperatureSensor. This enables that all temperatureSensors can receive current valve opening and adjust temperature values to mock a working heater in the room.
 
Running and arguments:
temperatureSensor -i <IP of broker> -p <port of broker> -s <sensorID 0-MAX(int)> -t <starting temperature>
 
## Building
All three deployable components need to be built separately.
With Eclipse or with command line from repository root like:
    
    make all -C ./thermostat/Debug/  
    make all -C ./temperatureSensor/Debug/
    make all -C ./radiatorValve/Debug/

## Running
Following commands can be used to run minimum setup:

Start MQTT Broker

    sudo docker run -it -p 1883:1883 --name=mosquitto  toke/mosquitto

Start mosquitte_sub to see and trace messages and content (optional)
 
    mosquitto_sub -t '#' -v

Start each deployable from repository root with following commands:

    gnome-terminal -e ./radiatorValve/Debug/radiatorValve -i LOCALHOST -p 1883
    gnome-terminal -e ./thermostat/Debug/thermostat -i LOCALHOST -p 1883 -t 25.2
    gnome-terminal -e ./temperatureSensor/Debug/temperatureSensor -i LOCALHOST -p 1883 -t 18 -s 1

Note:
-t provides target (thermostat) or start (temperatureSensor) temperature in degrees (float)
-s provides sensorID. Needs to be unique in case multiple sensors are started

Up to 12 temperatureSensors can be started but each need to have unique sensorID given with -s argument
 
##Tools, libraries and borrowed code
###mosquitto broker
 This was the recommended broker and was run in Docker container. No alternatives were tested or analyzed.
 
### mosquitto_pub and mosquitto_sub (https://github.com/eclipse/mosquitto)
 
 Handy tools to test parts of the system independently and manually. Used for tracing MQTT messaging.

###Mosquitto client for MQTT 3.1 and 3.1.1 (https://github.com/eclipse/mosquitto)
C Client library was used. Selected mainly to 100% avoid compatibility issues with mosquitto broker. With quick look did not seem like simplest API though. E.g. HiveMQ looked bit nicer. No time for any performance comparisons and that was not critical in this exercise.

Compiled from GIT sources and built library and related .h file was included/copied in to the repository to folder /externals/mosquittoClient

ISSUE: Due to above solution compilation of application is possible only for x86/Linux unless replaced with target version mosquitto client is cloned from GIT and recompiled for other targets.
TODO: Get/Clone and build during compilation of HeatingControlPrototype applications

###cJSON (https://github.com/DaveGamble/cJSON)
Simple JSON parser for C. Contains single .c and .h files which were included/copied in to the projects in the repository. 


 