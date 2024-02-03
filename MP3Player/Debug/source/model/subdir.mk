################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../source/model/evQueue.c \
../source/model/fsm_motor.c \
../source/model/fsm_states.c 

C_DEPS += \
./source/model/evQueue.d \
./source/model/fsm_motor.d \
./source/model/fsm_states.d 

OBJS += \
./source/model/evQueue.o \
./source/model/fsm_motor.o \
./source/model/fsm_states.o 


# Each subdirectory must supply rules for building sources it contributes
source/model/%.o: ../source/model/%.c source/model/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DCPU_MK64FN1M0VLL12 -D__USE_CMSIS -DDEBUG -I../source -I../source/lib -I../source/controls -I../source/model -I../source/MCAL -I../source/HAL -I../ -I../SDK/CMSIS -I../SDK/startup -O0 -fno-common -g3 -Wall -c -ffunction-sections -fdata-sections -ffreestanding -fno-builtin -fmerge-constants -fmacro-prefix-map="$(<D)/"= -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-source-2f-model

clean-source-2f-model:
	-$(RM) ./source/model/evQueue.d ./source/model/evQueue.o ./source/model/fsm_motor.d ./source/model/fsm_motor.o ./source/model/fsm_states.d ./source/model/fsm_states.o

.PHONY: clean-source-2f-model

