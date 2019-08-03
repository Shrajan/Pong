################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../DE1SoC_Servo/DE1SoC_Servo.c 

C_DEPS += \
./DE1SoC_Servo/DE1SoC_Servo.d 

OBJS += \
./DE1SoC_Servo/DE1SoC_Servo.o 


# Each subdirectory must supply rules for building sources it contributes
DE1SoC_Servo/%.o: ../DE1SoC_Servo/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM C Compiler 5'
	armcc --cpu=Cortex-A9.no_neon.no_vfp --arm -O0 -g --md --depend_format=unix_escaped --no_depend_system_headers --depend_dir="DE1SoC_Servo" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


