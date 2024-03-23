################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include utilities/subdir.mk
-include startup/subdir.mk
-include source/spectrum_display/subdir.mk
-include source/lcd_display/subdir.mk
-include source/equalizer/subdir.mk
-include source/audio_manager/subdir.mk
-include source/subdir.mk
-include lib/sdmmc/src/subdir.mk
-include lib/sdmmc/osa/subdir.mk
-include lib/sdmmc/host/subdir.mk
-include lib/mp3decoder/subdir.mk
-include lib/id3tagParser/subdir.mk
-include lib/helix/real/subdir.mk
-include lib/helix/subdir.mk
-include lib/fatfs/source/fsl_sd_disk/subdir.mk
-include lib/fatfs/source/fsl_ram_disk/subdir.mk
-include lib/fatfs/source/subdir.mk
-include lib/subdir.mk
-include drivers/MCAL/subdir.mk
-include drivers/HAL/subdir.mk
-include drivers/FSL/subdir.mk
-include device/subdir.mk
-include component/uart/subdir.mk
-include component/osa/subdir.mk
-include component/lists/subdir.mk
-include board/subdir.mk
ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

OPTIONAL_TOOL_DEPS := \
$(wildcard ../makefile.defs) \
$(wildcard ../makefile.init) \
$(wildcard ../makefile.targets) \


BUILD_ARTIFACT_NAME := frdmk64f_sdcard_fatfs
BUILD_ARTIFACT_EXTENSION := axf
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all:
	+@$(MAKE) --no-print-directory main-build && $(MAKE) --no-print-directory post-build

# Main-build Target
main-build: frdmk64f_sdcard_fatfs.axf

# Tool invocations
frdmk64f_sdcard_fatfs.axf: $(OBJS) $(USER_OBJS) makefile $(OPTIONAL_TOOL_DEPS)
	@echo 'Building target: $@'
	@echo 'Invoking: MCU Linker'
	arm-none-eabi-gcc -nostdlib -L"/frdmk64f_sdcard_fatfs/lib" -L"D:\00-ITBA\LabdeMicros\FINAL\Eclipse\mp3_eclipse\frdmk64f_sdcard_fatfs\lib" -Xlinker -Map="frdmk64f_sdcard_fatfs.map" -Xlinker --gc-sections -Xlinker -print-memory-usage -Xlinker --sort-section=alignment -Xlinker --cref -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -T frdmk64f_sdcard_fatfs_Debug.ld -o "frdmk64f_sdcard_fatfs.axf" $(OBJS) $(USER_OBJS) $(LIBS) -larm_cortexM4lf_math
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) frdmk64f_sdcard_fatfs.axf
	-@echo ' '

post-build:
	-@echo 'Performing post-build steps'
	-arm-none-eabi-size "frdmk64f_sdcard_fatfs.axf"; # arm-none-eabi-objcopy -v -O binary "frdmk64f_sdcard_fatfs.axf" "frdmk64f_sdcard_fatfs.bin" ; # checksum -p MK64FN1M0xxx12 -d "frdmk64f_sdcard_fatfs.bin";
	-@echo ' '

.PHONY: all clean dependents main-build post-build

-include ../makefile.targets
