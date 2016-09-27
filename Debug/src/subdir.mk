################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/HelloWord.cpp \
../src/b64.cpp \
../src/bmh.cpp \
../src/funcoes.cpp \
../src/server.cpp \
../src/sha.cpp \
../src/thread.cpp 

OBJS += \
./src/HelloWord.o \
./src/b64.o \
./src/bmh.o \
./src/funcoes.o \
./src/server.o \
./src/sha.o \
./src/thread.o 

CPP_DEPS += \
./src/HelloWord.d \
./src/b64.d \
./src/bmh.d \
./src/funcoes.d \
./src/server.d \
./src/sha.d \
./src/thread.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


