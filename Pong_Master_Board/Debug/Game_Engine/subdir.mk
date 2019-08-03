################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Game_Engine/Game_Engine.c 

C_DEPS += \
./Game_Engine/Game_Engine.d 

OBJS += \
./Game_Engine/Game_Engine.o 


# Each subdirectory must supply rules for building sources it contributes
Game_Engine/%.o: ../Game_Engine/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM C Compiler 5'
	armcc --cpu=Cortex-A9.no_neon --apcs=/hardfp --arm -O0 -g --md --depend_format=unix_escaped --no_depend_system_headers --depend_dir="Game_Engine" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


