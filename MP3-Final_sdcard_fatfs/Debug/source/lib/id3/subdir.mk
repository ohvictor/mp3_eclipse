################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../source/lib/id3/read_id3.c 

C_DEPS += \
./source/lib/id3/read_id3.d 

OBJS += \
./source/lib/id3/read_id3.o 


# Each subdirectory must supply rules for building sources it contributes
source/lib/id3/%.o: ../source/lib/id3/%.c source/lib/id3/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -std=gnu99 -D__REDLIB__ -DCPU_MK64FN1M0VLL12 -DCPU_MK64FN1M0VLL12_cm4 -DFRDM_K64F -DFREEDOM -DSD_ENABLED -DMCUXPRESSO_SDK -DSDK_DEBUGCONSOLE=0 -DCR_INTEGER_PRINTF -DPRINTF_FLOAT_ENABLE=0 -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -I"C:\Users\fn_mo\Documents\MCUXpressoIDE_11.8.0_1165\workspace\MP3-Final_sdcard_fatfs\source" -I"C:\Users\fn_mo\Documents\MCUXpressoIDE_11.8.0_1165\workspace\MP3-Final_sdcard_fatfs\source\lib\helix\pub" -I"C:\Users\fn_mo\Documents\MCUXpressoIDE_11.8.0_1165\workspace\MP3-Final_sdcard_fatfs\startup" -I"C:\Users\fn_mo\Documents\MCUXpressoIDE_11.8.0_1165\workspace\MP3-Final_sdcard_fatfs\drivers" -I"C:\Users\fn_mo\Documents\MCUXpressoIDE_11.8.0_1165\workspace\MP3-Final_sdcard_fatfs\drivers\fsl" -I"C:\Users\fn_mo\Documents\MCUXpressoIDE_11.8.0_1165\workspace\MP3-Final_sdcard_fatfs\drivers\HAL" -I"C:\Users\fn_mo\Documents\MCUXpressoIDE_11.8.0_1165\workspace\MP3-Final_sdcard_fatfs\drivers\MCAL" -I"C:\Users\fn_mo\Documents\MCUXpressoIDE_11.8.0_1165\workspace\MP3-Final_sdcard_fatfs\source\FSYS\filesystem" -I"C:\Users\fn_mo\Documents\MCUXpressoIDE_11.8.0_1165\workspace\MP3-Final_sdcard_fatfs\source\FSYS\mp3decoder" -I"C:\Users\fn_mo\Documents\MCUXpressoIDE_11.8.0_1165\workspace\MP3-Final_sdcard_fatfs\source\lib" -I"C:\Users\fn_mo\Documents\MCUXpressoIDE_11.8.0_1165\workspace\MP3-Final_sdcard_fatfs\source\lib\fatfs" -I"C:\Users\fn_mo\Documents\MCUXpressoIDE_11.8.0_1165\workspace\MP3-Final_sdcard_fatfs\source\lib\fatfs\source" -I"C:\Users\fn_mo\Documents\MCUXpressoIDE_11.8.0_1165\workspace\MP3-Final_sdcard_fatfs\source\lib\fatfs\source\fsl_sd_disk" -I"C:\Users\fn_mo\Documents\MCUXpressoIDE_11.8.0_1165\workspace\MP3-Final_sdcard_fatfs\source\lib\helix" -I"C:\Users\fn_mo\Documents\MCUXpressoIDE_11.8.0_1165\workspace\MP3-Final_sdcard_fatfs\source\lib\id3" -I"C:\Users\fn_mo\Documents\MCUXpressoIDE_11.8.0_1165\workspace\MP3-Final_sdcard_fatfs\source\LOGIC" -I"C:\Users\fn_mo\Documents\MCUXpressoIDE_11.8.0_1165\workspace\MP3-Final_sdcard_fatfs\source\LOGIC\audioplayer" -I"C:\Users\fn_mo\Documents\MCUXpressoIDE_11.8.0_1165\workspace\MP3-Final_sdcard_fatfs\source\UI" -I"C:\Users\fn_mo\Documents\MCUXpressoIDE_11.8.0_1165\workspace\MP3-Final_sdcard_fatfs\sdmmc\inc" -I"C:\Users\fn_mo\Documents\MCUXpressoIDE_11.8.0_1165\workspace\MP3-Final_sdcard_fatfs\sdmmc\host" -I"C:\Users\fn_mo\Documents\MCUXpressoIDE_11.8.0_1165\workspace\MP3-Final_sdcard_fatfs\utilities" -I"C:\Users\fn_mo\Documents\MCUXpressoIDE_11.8.0_1165\workspace\MP3-Final_sdcard_fatfs\device" -I"C:\Users\fn_mo\Documents\MCUXpressoIDE_11.8.0_1165\workspace\MP3-Final_sdcard_fatfs\component\uart" -I"C:\Users\fn_mo\Documents\MCUXpressoIDE_11.8.0_1165\workspace\MP3-Final_sdcard_fatfs\component\lists" -I"C:\Users\fn_mo\Documents\MCUXpressoIDE_11.8.0_1165\workspace\MP3-Final_sdcard_fatfs\CMSIS" -I"C:\Users\fn_mo\Documents\MCUXpressoIDE_11.8.0_1165\workspace\MP3-Final_sdcard_fatfs\sdmmc\osa" -I"C:\Users\fn_mo\Documents\MCUXpressoIDE_11.8.0_1165\workspace\MP3-Final_sdcard_fatfs\component\osa" -I"C:\Users\fn_mo\Documents\MCUXpressoIDE_11.8.0_1165\workspace\MP3-Final_sdcard_fatfs\board" -I"C:\Users\fn_mo\Documents\MCUXpressoIDE_11.8.0_1165\workspace\MP3-Final_sdcard_fatfs\frdmk64f\sdmmc_examples\sdcard_fatfs" -O0 -fno-common -g3 -c -ffunction-sections -fdata-sections -ffreestanding -fno-builtin -fmerge-constants -fmacro-prefix-map="$(<D)/"= -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-source-2f-lib-2f-id3

clean-source-2f-lib-2f-id3:
	-$(RM) ./source/lib/id3/read_id3.d ./source/lib/id3/read_id3.o

.PHONY: clean-source-2f-lib-2f-id3

