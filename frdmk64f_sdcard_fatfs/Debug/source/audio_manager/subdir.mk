################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../source/audio_manager/audio_manager.c 

C_DEPS += \
./source/audio_manager/audio_manager.d 

OBJS += \
./source/audio_manager/audio_manager.o 


# Each subdirectory must supply rules for building sources it contributes
source/audio_manager/%.o: ../source/audio_manager/%.c source/audio_manager/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -std=gnu99 -D__REDLIB__ -DCPU_MK64FN1M0VLL12 -DCPU_MK64FN1M0VLL12_cm4 -DFRDM_K64F -DFREEDOM -DSD_ENABLED -DMCUXPRESSO_SDK -DCR_INTEGER_PRINTF -DPRINTF_FLOAT_ENABLE=0 -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -DSDK_DEBUGCONSOLE=0 -I"D:\00-ITBA\LabdeMicros\FINAL\Eclipse\mp3_eclipse\frdmk64f_sdcard_fatfs\board" -I"D:\00-ITBA\LabdeMicros\FINAL\Eclipse\mp3_eclipse\frdmk64f_sdcard_fatfs\source" -I"D:\00-ITBA\LabdeMicros\FINAL\Eclipse\mp3_eclipse\frdmk64f_sdcard_fatfs\source\audio_manager" -I"D:\00-ITBA\LabdeMicros\FINAL\Eclipse\mp3_eclipse\frdmk64f_sdcard_fatfs\source\equalizer" -I"D:\00-ITBA\LabdeMicros\FINAL\Eclipse\mp3_eclipse\frdmk64f_sdcard_fatfs\source\spectrum_display" -I"D:\00-ITBA\LabdeMicros\FINAL\Eclipse\mp3_eclipse\frdmk64f_sdcard_fatfs\startup" -I"D:\00-ITBA\LabdeMicros\FINAL\Eclipse\mp3_eclipse\frdmk64f_sdcard_fatfs\drivers" -I"D:\00-ITBA\LabdeMicros\FINAL\Eclipse\mp3_eclipse\frdmk64f_sdcard_fatfs\drivers\HAL" -I"D:\00-ITBA\LabdeMicros\FINAL\Eclipse\mp3_eclipse\frdmk64f_sdcard_fatfs\drivers\MCAL" -I"D:\00-ITBA\LabdeMicros\FINAL\Eclipse\mp3_eclipse\frdmk64f_sdcard_fatfs\drivers\FSL" -I"D:\00-ITBA\LabdeMicros\FINAL\Eclipse\mp3_eclipse\frdmk64f_sdcard_fatfs\utilities" -I"D:\00-ITBA\LabdeMicros\FINAL\Eclipse\mp3_eclipse\frdmk64f_sdcard_fatfs\CMSIS" -I"D:\00-ITBA\LabdeMicros\FINAL\Eclipse\mp3_eclipse\frdmk64f_sdcard_fatfs\device" -I"D:\00-ITBA\LabdeMicros\FINAL\Eclipse\mp3_eclipse\frdmk64f_sdcard_fatfs\component\osa" -I"D:\00-ITBA\LabdeMicros\FINAL\Eclipse\mp3_eclipse\frdmk64f_sdcard_fatfs\component\uart" -I"D:\00-ITBA\LabdeMicros\FINAL\Eclipse\mp3_eclipse\frdmk64f_sdcard_fatfs\component\lists" -I"D:\00-ITBA\LabdeMicros\FINAL\Eclipse\mp3_eclipse\frdmk64f_sdcard_fatfs\lib\sdmmc\inc" -I"D:\00-ITBA\LabdeMicros\FINAL\Eclipse\mp3_eclipse\frdmk64f_sdcard_fatfs\lib\sdmmc\host" -I"D:\00-ITBA\LabdeMicros\FINAL\Eclipse\mp3_eclipse\frdmk64f_sdcard_fatfs\lib\sdmmc\osa" -I"D:\00-ITBA\LabdeMicros\FINAL\Eclipse\mp3_eclipse\frdmk64f_sdcard_fatfs\lib\fatfs\source" -I"D:\00-ITBA\LabdeMicros\FINAL\Eclipse\mp3_eclipse\frdmk64f_sdcard_fatfs\lib\fatfs\source\fsl_ram_disk" -I"D:\00-ITBA\LabdeMicros\FINAL\Eclipse\mp3_eclipse\frdmk64f_sdcard_fatfs\lib\fatfs\source\fsl_sd_disk" -I"D:\00-ITBA\LabdeMicros\FINAL\Eclipse\mp3_eclipse\frdmk64f_sdcard_fatfs\lib\helix" -I"D:\00-ITBA\LabdeMicros\FINAL\Eclipse\mp3_eclipse\frdmk64f_sdcard_fatfs\lib\helix\pub" -I"D:\00-ITBA\LabdeMicros\FINAL\Eclipse\mp3_eclipse\frdmk64f_sdcard_fatfs\lib\id3tagParser" -I"D:\00-ITBA\LabdeMicros\FINAL\Eclipse\mp3_eclipse\frdmk64f_sdcard_fatfs\lib\mp3decoder" -I"D:\00-ITBA\LabdeMicros\FINAL\Eclipse\mp3_eclipse\frdmk64f_sdcard_fatfs\lib" -I"D:\00-ITBA\LabdeMicros\FINAL\Eclipse\mp3_eclipse\frdmk64f_sdcard_fatfs\source\spectrum_display" -O0 -fno-common -g3 -c -ffunction-sections -fdata-sections -ffreestanding -fno-builtin -fmerge-constants -fmacro-prefix-map="$(<D)/"= -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-source-2f-audio_manager

clean-source-2f-audio_manager:
	-$(RM) ./source/audio_manager/audio_manager.d ./source/audio_manager/audio_manager.o

.PHONY: clean-source-2f-audio_manager

