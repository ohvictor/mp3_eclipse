################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../drivers/MCAL/spi/spi_master.c 

OBJS += \
./drivers/MCAL/spi/spi_master.o 

C_DEPS += \
./drivers/MCAL/spi/spi_master.d 


# Each subdirectory must supply rules for building sources it contributes
drivers/MCAL/spi/%.o: ../drivers/MCAL/spi/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -std=gnu99 -D__REDLIB__ -DCPU_MK64FN1M0VLL12 -DCPU_MK64FN1M0VLL12_cm4 -DFRDM_K64F -DFREEDOM -DSD_ENABLED -DMCUXPRESSO_SDK -DCR_INTEGER_PRINTF -DPRINTF_FLOAT_ENABLE=0 -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -DSDK_DEBUGCONSOLE=0 -I"D:\Documents\repos\mp3player\workspace\mp3player_bm\board" -I"D:\Documents\repos\mp3player\workspace\mp3player_bm\source" -I"D:\Documents\repos\mp3player\workspace\mp3player_bm\source\audio_manager" -I"D:\Documents\repos\mp3player\workspace\mp3player_bm\source\equalizer" -I"D:\Documents\repos\mp3player\workspace\mp3player_bm\source\spectrum_display" -I"D:\Documents\repos\mp3player\workspace\mp3player_bm\startup" -I"D:\Documents\repos\mp3player\workspace\mp3player_bm\drivers" -I"D:\Documents\repos\mp3player\workspace\mp3player_bm\drivers\HAL" -I"D:\Documents\repos\mp3player\workspace\mp3player_bm\drivers\MCAL" -I"D:\Documents\repos\mp3player\workspace\mp3player_bm\drivers\FSL" -I"D:\Documents\repos\mp3player\workspace\mp3player_bm\utilities" -I"D:\Documents\repos\mp3player\workspace\mp3player_bm\CMSIS" -I"D:\Documents\repos\mp3player\workspace\mp3player_bm\device" -I"D:\Documents\repos\mp3player\workspace\mp3player_bm\component\osa" -I"D:\Documents\repos\mp3player\workspace\mp3player_bm\component\uart" -I"D:\Documents\repos\mp3player\workspace\mp3player_bm\component\lists" -I"D:\Documents\repos\mp3player\workspace\mp3player_bm\lib\sdmmc\inc" -I"D:\Documents\repos\mp3player\workspace\mp3player_bm\lib\sdmmc\host" -I"D:\Documents\repos\mp3player\workspace\mp3player_bm\lib\sdmmc\osa" -I"D:\Documents\repos\mp3player\workspace\mp3player_bm\lib\fatfs\source" -I"D:\Documents\repos\mp3player\workspace\mp3player_bm\lib\fatfs\source\fsl_ram_disk" -I"D:\Documents\repos\mp3player\workspace\mp3player_bm\lib\fatfs\source\fsl_sd_disk" -I"D:\Documents\repos\mp3player\workspace\mp3player_bm\lib\helix" -I"D:\Documents\repos\mp3player\workspace\mp3player_bm\lib\helix\pub" -I"D:\Documents\repos\mp3player\workspace\mp3player_bm\lib\id3tagParser" -I"D:\Documents\repos\mp3player\workspace\mp3player_bm\lib\mp3decoder" -I"D:\Documents\repos\mp3player\workspace\mp3player_bm\lib" -I"D:\Documents\repos\mp3player\workspace\mp3player_bm\source\spectrum_display" -O0 -fno-common -g3 -c -ffunction-sections -fdata-sections -ffreestanding -fno-builtin -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -D__REDLIB__ -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


