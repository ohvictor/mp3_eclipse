################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../source/HAL/DRVLEDDISPLAY.c \
../source/HAL/displayBoard.c \
../source/HAL/encoder.c 

OBJS += \
./source/HAL/DRVLEDDISPLAY.o \
./source/HAL/displayBoard.o \
./source/HAL/encoder.o 

C_DEPS += \
./source/HAL/DRVLEDDISPLAY.d \
./source/HAL/displayBoard.d \
./source/HAL/encoder.d 


# Each subdirectory must supply rules for building sources it contributes
source/HAL/%.o: ../source/HAL/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DCPU_MK64FN1M0VLL12 -D__USE_CMSIS -DDEBUG -I../source -I../source/lib -I../source/controls -I../source/model -I../source/MCAL -I../source/HAL -I../ -I../SDK/CMSIS -I../SDK/startup -O0 -fno-common -g3 -Wall -c -ffunction-sections -fdata-sections -ffreestanding -fno-builtin -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


