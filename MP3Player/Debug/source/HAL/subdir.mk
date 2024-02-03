################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../source/HAL/DRVLEDDISPLAY.c \
../source/HAL/displayBoard.c \
../source/HAL/encoder.c 

C_DEPS += \
./source/HAL/DRVLEDDISPLAY.d \
./source/HAL/displayBoard.d \
./source/HAL/encoder.d 

OBJS += \
./source/HAL/DRVLEDDISPLAY.o \
./source/HAL/displayBoard.o \
./source/HAL/encoder.o 


# Each subdirectory must supply rules for building sources it contributes
source/HAL/%.o: ../source/HAL/%.c source/HAL/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DCPU_MK64FN1M0VLL12 -D__USE_CMSIS -DDEBUG -I../source -I../source/lib -I../source/controls -I../source/model -I../source/MCAL -I../source/HAL -I../ -I../SDK/CMSIS -I../SDK/startup -O0 -fno-common -g3 -Wall -c -ffunction-sections -fdata-sections -ffreestanding -fno-builtin -fmerge-constants -fmacro-prefix-map="$(<D)/"= -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-source-2f-HAL

clean-source-2f-HAL:
	-$(RM) ./source/HAL/DRVLEDDISPLAY.d ./source/HAL/DRVLEDDISPLAY.o ./source/HAL/displayBoard.d ./source/HAL/displayBoard.o ./source/HAL/encoder.d ./source/HAL/encoder.o

.PHONY: clean-source-2f-HAL

