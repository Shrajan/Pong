################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../DE1SoC_WM8731/DE1SoC_WM8731.c 

C_DEPS += \
./DE1SoC_WM8731/DE1SoC_WM8731.d 

OBJS += \
./DE1SoC_WM8731/DE1SoC_WM8731.o 


# Each subdirectory must supply rules for building sources it contributes
DE1SoC_WM8731/%.o: ../DE1SoC_WM8731/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM C Compiler 5'
	armcc --cpu=Cortex-A9.no_neon.no_vfp --arm -O0 -g --md --depend_format=unix_escaped --no_depend_system_headers --depend_dir="DE1SoC_WM8731" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


