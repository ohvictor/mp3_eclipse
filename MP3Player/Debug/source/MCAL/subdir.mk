################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../source/MCAL/FTM.c \
../source/MCAL/PORT.c \
../source/MCAL/eDMA.c \
../source/MCAL/fileSystem.c \
../source/MCAL/gpio.c \
../source/MCAL/node.c \
../source/MCAL/systick.c \
../source/MCAL/timer.c 

C_DEPS += \
./source/MCAL/FTM.d \
./source/MCAL/PORT.d \
./source/MCAL/eDMA.d \
./source/MCAL/fileSystem.d \
./source/MCAL/gpio.d \
./source/MCAL/node.d \
./source/MCAL/systick.d \
./source/MCAL/timer.d 

OBJS += \
./source/MCAL/FTM.o \
./source/MCAL/PORT.o \
./source/MCAL/eDMA.o \
./source/MCAL/fileSystem.o \
./source/MCAL/gpio.o \
./source/MCAL/node.o \
./source/MCAL/systick.o \
./source/MCAL/timer.o 


# Each subdirectory must supply rules for building sources it contributes
source/MCAL/%.o: ../source/MCAL/%.c source/MCAL/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DCPU_MK64FN1M0VLL12 -D__USE_CMSIS -DDEBUG -I../source -I../source/lib -I../source/controls -I../source/model -I../source/MCAL -I../source/HAL -I../ -I../SDK/CMSIS -I../SDK/startup -O0 -fno-common -g3 -Wall -c -ffunction-sections -fdata-sections -ffreestanding -fno-builtin -fmerge-constants -fmacro-prefix-map="$(<D)/"= -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-source-2f-MCAL

clean-source-2f-MCAL:
	-$(RM) ./source/MCAL/FTM.d ./source/MCAL/FTM.o ./source/MCAL/PORT.d ./source/MCAL/PORT.o ./source/MCAL/eDMA.d ./source/MCAL/eDMA.o ./source/MCAL/fileSystem.d ./source/MCAL/fileSystem.o ./source/MCAL/gpio.d ./source/MCAL/gpio.o ./source/MCAL/node.d ./source/MCAL/node.o ./source/MCAL/systick.d ./source/MCAL/systick.o ./source/MCAL/timer.d ./source/MCAL/timer.o

.PHONY: clean-source-2f-MCAL

