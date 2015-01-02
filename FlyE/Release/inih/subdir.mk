################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../inih/ini.c 

OBJS += \
./inih/ini.o 

C_DEPS += \
./inih/ini.d 


# Each subdirectory must supply rules for building sources it contributes
inih/%.o: ../inih/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -o "$@" "$<"  -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)"
	@echo 'Finished building: $<'
	@echo ' '


