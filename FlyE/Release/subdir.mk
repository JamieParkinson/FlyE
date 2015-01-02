################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../AcceleratorGeometry.cpp \
../AntiHydrogen.cpp \
../ConfigLoader.cpp \
../Electrode.cpp \
../ElectrodeLocator.cpp \
../FlyE.cpp \
../IntegerDistribution.cpp \
../Particle.cpp \
../Simulator.cpp \
../SmartField.cpp \
../SubConfig.cpp \
../VectorField.cpp \
../VoltageScheme.cpp \
../Writer.cpp \
../runFlyE.cpp 

OBJS += \
./AcceleratorGeometry.o \
./AntiHydrogen.o \
./ConfigLoader.o \
./Electrode.o \
./ElectrodeLocator.o \
./FlyE.o \
./IntegerDistribution.o \
./Particle.o \
./Simulator.o \
./SmartField.o \
./SubConfig.o \
./VectorField.o \
./VoltageScheme.o \
./Writer.o \
./runFlyE.o 

CPP_DEPS += \
./AcceleratorGeometry.d \
./AntiHydrogen.d \
./ConfigLoader.d \
./Electrode.d \
./ElectrodeLocator.d \
./FlyE.d \
./IntegerDistribution.d \
./Particle.d \
./Simulator.d \
./SmartField.d \
./SubConfig.d \
./VectorField.d \
./VoltageScheme.d \
./Writer.d \
./runFlyE.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -o "$@" "$<" -std=c++0x -O3 -march=native -pipe -DBZ_THREADSAFE -D_GLIBCXX_PARALLEL -g -Wall -c -fmessage-length=0 -lblitz -fopenmp -L/usr/lib/x86_64-linux-gnu -Wl,-z,relro -lpthread -lz -ldl -lm -v -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)"
	@echo 'Finished building: $<'
	@echo ' '


