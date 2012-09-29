################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/Branch.cpp \
../src/BranchLevel.cpp \
../src/BranchSegment.cpp \
../src/OnlineVariance.cpp \
../src/Random.cpp \
../src/construct.cpp \
../src/extract.cpp \
../src/main.cpp \
../src/parse.cpp \
../src/util.cpp 

OBJS += \
./src/Branch.o \
./src/BranchLevel.o \
./src/BranchSegment.o \
./src/OnlineVariance.o \
./src/Random.o \
./src/construct.o \
./src/extract.o \
./src/main.o \
./src/parse.o \
./src/util.o 

CPP_DEPS += \
./src/Branch.d \
./src/BranchLevel.d \
./src/BranchSegment.d \
./src/OnlineVariance.d \
./src/Random.d \
./src/construct.d \
./src/extract.d \
./src/main.d \
./src/parse.d \
./src/util.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I../include/rapidxml-1.13 -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


