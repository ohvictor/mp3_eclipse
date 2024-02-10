################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../drivers/fsl/fsl_clock.c \
../drivers/fsl/fsl_common.c \
../drivers/fsl/fsl_common_arm.c \
../drivers/fsl/fsl_ftfx_cache.c \
../drivers/fsl/fsl_ftfx_controller.c \
../drivers/fsl/fsl_ftfx_flash.c \
../drivers/fsl/fsl_ftfx_flexnvm.c \
../drivers/fsl/fsl_gpio.c \
../drivers/fsl/fsl_sdhc.c \
../drivers/fsl/fsl_smc.c \
../drivers/fsl/fsl_sysmpu.c \
../drivers/fsl/fsl_uart.c 

C_DEPS += \
./drivers/fsl/fsl_clock.d \
./drivers/fsl/fsl_common.d \
./drivers/fsl/fsl_common_arm.d \
./drivers/fsl/fsl_ftfx_cache.d \
./drivers/fsl/fsl_ftfx_controller.d \
./drivers/fsl/fsl_ftfx_flash.d \
./drivers/fsl/fsl_ftfx_flexnvm.d \
./drivers/fsl/fsl_gpio.d \
./drivers/fsl/fsl_sdhc.d \
./drivers/fsl/fsl_smc.d \
./drivers/fsl/fsl_sysmpu.d \
./drivers/fsl/fsl_uart.d 

OBJS += \
./drivers/fsl/fsl_clock.o \
./drivers/fsl/fsl_common.o \
./drivers/fsl/fsl_common_arm.o \
./drivers/fsl/fsl_ftfx_cache.o \
./drivers/fsl/fsl_ftfx_controller.o \
./drivers/fsl/fsl_ftfx_flash.o \
./drivers/fsl/fsl_ftfx_flexnvm.o \
./drivers/fsl/fsl_gpio.o \
./drivers/fsl/fsl_sdhc.o \
./drivers/fsl/fsl_smc.o \
./drivers/fsl/fsl_sysmpu.o \
./drivers/fsl/fsl_uart.o 


# Each subdirectory must supply rules for building sources it contributes
drivers/fsl/%.o: ../drivers/fsl/%.c drivers/fsl/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -std=gnu99 -D__REDLIB__ -DCPU_MK64FN1M0VLL12 -DCPU_MK64FN1M0VLL12_cm4 -DFRDM_K64F -DFREEDOM -DSD_ENABLED -DMCUXPRESSO_SDK -DSDK_DEBUGCONSOLE=0 -DCR_INTEGER_PRINTF -DPRINTF_FLOAT_ENABLE=0 -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -I"C:\Users\fn_mo\Documents\MCUXpressoIDE_11.8.0_1165\workspace\MP3-Final_sdcard_fatfs\source" -I"C:\Users\fn_mo\Documents\MCUXpressoIDE_11.8.0_1165\workspace\MP3-Final_sdcard_fatfs\source\lib\helix\pub" -I"C:\Users\fn_mo\Documents\MCUXpressoIDE_11.8.0_1165\workspace\MP3-Final_sdcard_fatfs\startup" -I"C:\Users\fn_mo\Documents\MCUXpressoIDE_11.8.0_1165\workspace\MP3-Final_sdcard_fatfs\drivers" -I"C:\Users\fn_mo\Documents\MCUXpressoIDE_11.8.0_1165\workspace\MP3-Final_sdcard_fatfs\drivers\fsl" -I"C:\Users\fn_mo\Documents\MCUXpressoIDE_11.8.0_1165\workspace\MP3-Final_sdcard_fatfs\drivers\HAL" -I"C:\Users\fn_mo\Documents\MCUXpressoIDE_11.8.0_1165\workspace\MP3-Final_sdcard_fatfs\drivers\MCAL" -I"C:\Users\fn_mo\Documents\MCUXpressoIDE_11.8.0_1165\workspace\MP3-Final_sdcard_fatfs\source\FSYS\filesystem" -I"C:\Users\fn_mo\Documents\MCUXpressoIDE_11.8.0_1165\workspace\MP3-Final_sdcard_fatfs\source\FSYS\mp3decoder" -I"C:\Users\fn_mo\Documents\MCUXpressoIDE_11.8.0_1165\workspace\MP3-Final_sdcard_fatfs\source\lib" -I"C:\Users\fn_mo\Documents\MCUXpressoIDE_11.8.0_1165\workspace\MP3-Final_sdcard_fatfs\source\lib\fatfs" -I"C:\Users\fn_mo\Documents\MCUXpressoIDE_11.8.0_1165\workspace\MP3-Final_sdcard_fatfs\source\lib\fatfs\source" -I"C:\Users\fn_mo\Documents\MCUXpressoIDE_11.8.0_1165\workspace\MP3-Final_sdcard_fatfs\source\lib\fatfs\source\fsl_sd_disk" -I"C:\Users\fn_mo\Documents\MCUXpressoIDE_11.8.0_1165\workspace\MP3-Final_sdcard_fatfs\source\lib\helix" -I"C:\Users\fn_mo\Documents\MCUXpressoIDE_11.8.0_1165\workspace\MP3-Final_sdcard_fatfs\source\lib\id3" -I"C:\Users\fn_mo\Documents\MCUXpressoIDE_11.8.0_1165\workspace\MP3-Final_sdcard_fatfs\source\LOGIC" -I"C:\Users\fn_mo\Documents\MCUXpressoIDE_11.8.0_1165\workspace\MP3-Final_sdcard_fatfs\source\LOGIC\audioplayer" -I"C:\Users\fn_mo\Documents\MCUXpressoIDE_11.8.0_1165\workspace\MP3-Final_sdcard_fatfs\source\UI" -I"C:\Users\fn_mo\Documents\MCUXpressoIDE_11.8.0_1165\workspace\MP3-Final_sdcard_fatfs\sdmmc\inc" -I"C:\Users\fn_mo\Documents\MCUXpressoIDE_11.8.0_1165\workspace\MP3-Final_sdcard_fatfs\sdmmc\host" -I"C:\Users\fn_mo\Documents\MCUXpressoIDE_11.8.0_1165\workspace\MP3-Final_sdcard_fatfs\utilities" -I"C:\Users\fn_mo\Documents\MCUXpressoIDE_11.8.0_1165\workspace\MP3-Final_sdcard_fatfs\device" -I"C:\Users\fn_mo\Documents\MCUXpressoIDE_11.8.0_1165\workspace\MP3-Final_sdcard_fatfs\component\uart" -I"C:\Users\fn_mo\Documents\MCUXpressoIDE_11.8.0_1165\workspace\MP3-Final_sdcard_fatfs\component\lists" -I"C:\Users\fn_mo\Documents\MCUXpressoIDE_11.8.0_1165\workspace\MP3-Final_sdcard_fatfs\CMSIS" -I"C:\Users\fn_mo\Documents\MCUXpressoIDE_11.8.0_1165\workspace\MP3-Final_sdcard_fatfs\sdmmc\osa" -I"C:\Users\fn_mo\Documents\MCUXpressoIDE_11.8.0_1165\workspace\MP3-Final_sdcard_fatfs\component\osa" -I"C:\Users\fn_mo\Documents\MCUXpressoIDE_11.8.0_1165\workspace\MP3-Final_sdcard_fatfs\board" -I"C:\Users\fn_mo\Documents\MCUXpressoIDE_11.8.0_1165\workspace\MP3-Final_sdcard_fatfs\frdmk64f\sdmmc_examples\sdcard_fatfs" -O0 -fno-common -g3 -c -ffunction-sections -fdata-sections -ffreestanding -fno-builtin -fmerge-constants -fmacro-prefix-map="$(<D)/"= -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-drivers-2f-fsl

clean-drivers-2f-fsl:
	-$(RM) ./drivers/fsl/fsl_clock.d ./drivers/fsl/fsl_clock.o ./drivers/fsl/fsl_common.d ./drivers/fsl/fsl_common.o ./drivers/fsl/fsl_common_arm.d ./drivers/fsl/fsl_common_arm.o ./drivers/fsl/fsl_ftfx_cache.d ./drivers/fsl/fsl_ftfx_cache.o ./drivers/fsl/fsl_ftfx_controller.d ./drivers/fsl/fsl_ftfx_controller.o ./drivers/fsl/fsl_ftfx_flash.d ./drivers/fsl/fsl_ftfx_flash.o ./drivers/fsl/fsl_ftfx_flexnvm.d ./drivers/fsl/fsl_ftfx_flexnvm.o ./drivers/fsl/fsl_gpio.d ./drivers/fsl/fsl_gpio.o ./drivers/fsl/fsl_sdhc.d ./drivers/fsl/fsl_sdhc.o ./drivers/fsl/fsl_smc.d ./drivers/fsl/fsl_smc.o ./drivers/fsl/fsl_sysmpu.d ./drivers/fsl/fsl_sysmpu.o ./drivers/fsl/fsl_uart.d ./drivers/fsl/fsl_uart.o

.PHONY: clean-drivers-2f-fsl

