################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../source/MCAL/SD/sd.c \
../source/MCAL/SD/sd_2.c 

C_DEPS += \
./source/MCAL/SD/sd.d \
./source/MCAL/SD/sd_2.d 

OBJS += \
./source/MCAL/SD/sd.o \
./source/MCAL/SD/sd_2.o 


# Each subdirectory must supply rules for building sources it contributes
source/MCAL/SD/%.o: ../source/MCAL/SD/%.c source/MCAL/SD/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DCPU_MK64FN1M0VLL12 -D__USE_CMSIS -DDEBUG -I../source -I../source/lib -I../source/controls -I../source/model -I../source/MCAL -I../source/HAL -I../ -I../SDK/CMSIS -I../SDK/startup -O0 -fno-common -g3 -Wall -c -ffunction-sections -fdata-sections -ffreestanding -fno-builtin -fmerge-constants -fmacro-prefix-map="$(<D)/"= -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-source-2f-MCAL-2f-SD

clean-source-2f-MCAL-2f-SD:
	-$(RM) ./source/MCAL/SD/sd.d ./source/MCAL/SD/sd.o ./source/MCAL/SD/sd_2.d ./source/MCAL/SD/sd_2.o

.PHONY: clean-source-2f-MCAL-2f-SD

