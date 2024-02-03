################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../source/lib/fatfs/diskio.c \
../source/lib/fatfs/ff.c \
../source/lib/fatfs/ffsystem.c \
../source/lib/fatfs/ffunicode.c 

C_DEPS += \
./source/lib/fatfs/diskio.d \
./source/lib/fatfs/ff.d \
./source/lib/fatfs/ffsystem.d \
./source/lib/fatfs/ffunicode.d 

OBJS += \
./source/lib/fatfs/diskio.o \
./source/lib/fatfs/ff.o \
./source/lib/fatfs/ffsystem.o \
./source/lib/fatfs/ffunicode.o 


# Each subdirectory must supply rules for building sources it contributes
source/lib/fatfs/%.o: ../source/lib/fatfs/%.c source/lib/fatfs/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DCPU_MK64FN1M0VLL12 -D__USE_CMSIS -DDEBUG -I../source -I../source/lib -I../source/controls -I../source/model -I../source/MCAL -I../source/HAL -I../ -I../SDK/CMSIS -I../SDK/startup -O0 -fno-common -g3 -Wall -c -ffunction-sections -fdata-sections -ffreestanding -fno-builtin -fmerge-constants -fmacro-prefix-map="$(<D)/"= -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-source-2f-lib-2f-fatfs

clean-source-2f-lib-2f-fatfs:
	-$(RM) ./source/lib/fatfs/diskio.d ./source/lib/fatfs/diskio.o ./source/lib/fatfs/ff.d ./source/lib/fatfs/ff.o ./source/lib/fatfs/ffsystem.d ./source/lib/fatfs/ffsystem.o ./source/lib/fatfs/ffunicode.d ./source/lib/fatfs/ffunicode.o

.PHONY: clean-source-2f-lib-2f-fatfs

