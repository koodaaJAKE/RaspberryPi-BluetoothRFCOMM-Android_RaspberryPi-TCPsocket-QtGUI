################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Bluetooth_RFCOMM.c \
../HRLVEZ0.c \
../LCD.c \
../SerializeDeserialize.c \
../TCP_Socket.c \
../main.c 

OBJS += \
./Bluetooth_RFCOMM.o \
./HRLVEZ0.o \
./LCD.o \
./SerializeDeserialize.o \
./TCP_Socket.o \
./main.o 

C_DEPS += \
./Bluetooth_RFCOMM.d \
./HRLVEZ0.d \
./LCD.d \
./SerializeDeserialize.d \
./TCP_Socket.d \
./main.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


