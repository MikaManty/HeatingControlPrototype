################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/cJSON.c \
../src/jsonHelpersSensor.c \
../src/mosquittoHelpersSensor.c \
../src/temperatureSensor.c 

OBJS += \
./src/cJSON.o \
./src/jsonHelpersSensor.o \
./src/mosquittoHelpersSensor.o \
./src/temperatureSensor.o 

C_DEPS += \
./src/cJSON.d \
./src/jsonHelpersSensor.d \
./src/mosquittoHelpersSensor.d \
./src/temperatureSensor.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/mika/git/HeatingControlPrototype/temperatureSensor/inc" -I"/home/mika/git/HeatingControlPrototype/externals/mosquittoClient/lib" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


