################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/Module.cpp \
../src/ModuleInstance.cpp \
../src/Production.cpp \
../src/construct.cpp \
../src/main.cpp \
../src/parse.cpp 

OBJS += \
./src/Module.o \
./src/ModuleInstance.o \
./src/Production.o \
./src/construct.o \
./src/main.o \
./src/parse.o 

CPP_DEPS += \
./src/Module.d \
./src/ModuleInstance.d \
./src/Production.d \
./src/construct.d \
./src/main.d \
./src/parse.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


