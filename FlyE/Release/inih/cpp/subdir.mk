################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../inih/cpp/INIReader.cpp 

OBJS += \
./inih/cpp/INIReader.o 

CPP_DEPS += \
./inih/cpp/INIReader.d 


# Each subdirectory must supply rules for building sources it contributes
inih/cpp/%.o: ../inih/cpp/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -o "$@" "$<" -std=c++0x -O3 -march=native -pipe -DBZ_THREADSAFE -D_GLIBCXX_PARALLEL -g -Wall -c -fmessage-length=0 -lblitz -fopenmp -L/usr/lib/x86_64-linux-gnu -Wl,-z,relro -lpthread -lz -ldl -lm -v -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)"
	@echo 'Finished building: $<'
	@echo ' '


