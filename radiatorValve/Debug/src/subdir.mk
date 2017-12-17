################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/cJSON.c \
../src/jsonHelpersRadiatorValve.c \
../src/mosquittoHelpersRadiatorValve.c \
../src/radiatorValve.c 

OBJS += \
./src/cJSON.o \
./src/jsonHelpersRadiatorValve.o \
./src/mosquittoHelpersRadiatorValve.o \
./src/radiatorValve.o 

C_DEPS += \
./src/cJSON.d \
./src/jsonHelpersRadiatorValve.d \
./src/mosquittoHelpersRadiatorValve.d \
./src/radiatorValve.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/mika/git/HeatingControlPrototype/radiatorValve/inc" -I"/home/mika/git/HeatingControlPrototype/externals/mosquittoClient/lib" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


