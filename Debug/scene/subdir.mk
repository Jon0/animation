################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../scene/Scene.cpp \
../scene/ViewSpline.cpp 

OBJS += \
./scene/Scene.o \
./scene/ViewSpline.o 

CPP_DEPS += \
./scene/Scene.d \
./scene/ViewSpline.d 


# Each subdirectory must supply rules for building sources it contributes
scene/%.o: ../scene/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -D__GXX_EXPERIMENTAL_CXX0X__ -O0 -g3 -Wall -c -fmessage-length=0 -std=c++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


