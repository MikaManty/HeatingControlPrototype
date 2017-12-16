################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/cJSON.c \
../src/jsonHelpersThermostat.c \
../src/mosquittoHelpersThermostat.c \
../src/thermostat.c 

OBJS += \
./src/cJSON.o \
./src/jsonHelpersThermostat.o \
./src/mosquittoHelpersThermostat.o \
./src/thermostat.o 

C_DEPS += \
./src/cJSON.d \
./src/jsonHelpersThermostat.d \
./src/mosquittoHelpersThermostat.d \
./src/thermostat.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/mika/git/HeatingControlPrototype/externals/mosquittoClient/lib" -I"/home/mika/git/HeatingControlPrototype/thermostat/inc" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


