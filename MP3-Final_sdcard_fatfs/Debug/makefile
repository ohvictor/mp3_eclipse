################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include utilities/subdir.mk
-include startup/subdir.mk
-include source/lib/id3/subdir.mk
-include source/lib/helix/real/subdir.mk
-include source/lib/helix/subdir.mk
-include source/lib/fatfs/source/fsl_sd_disk/subdir.mk
-include source/lib/fatfs/source/subdir.mk
-include source/UI/subdir.mk
-include source/LOGIC/audioplayer/subdir.mk
-include source/LOGIC/subdir.mk
-include source/FSYS/mp3decoder/subdir.mk
-include source/FSYS/filesystem/subdir.mk
-include source/subdir.mk
-include sdmmc/src/subdir.mk
-include sdmmc/osa/subdir.mk
-include sdmmc/host/subdir.mk
-include drivers/fsl/subdir.mk
-include drivers/MCAL/subdir.mk
-include drivers/HAL/subdir.mk
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


BUILD_ARTIFACT_NAME := MP3-Final_sdcard_fatfs
BUILD_ARTIFACT_EXTENSION := axf
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all:
	+@$(MAKE) --no-print-directory main-build && $(MAKE) --no-print-directory post-build

# Main-build Target
main-build: MP3-Final_sdcard_fatfs.axf

# Tool invocations
MP3-Final_sdcard_fatfs.axf: $(OBJS) $(USER_OBJS) makefile $(OPTIONAL_TOOL_DEPS)
	@echo 'Building target: $@'
	@echo 'Invoking: MCU Linker'
	arm-none-eabi-gcc -nostdlib -Xlinker -Map="MP3-Final_sdcard_fatfs.map" -Xlinker --gc-sections -Xlinker -print-memory-usage -Xlinker --sort-section=alignment -Xlinker --cref -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -T MP3-Final_sdcard_fatfs_Debug.ld -o "MP3-Final_sdcard_fatfs.axf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) MP3-Final_sdcard_fatfs.axf
	-@echo ' '

post-build:
	-@echo 'Performing post-build steps'
	-arm-none-eabi-size "MP3-Final_sdcard_fatfs.axf"; # arm-none-eabi-objcopy -v -O binary "MP3-Final_sdcard_fatfs.axf" "MP3-Final_sdcard_fatfs.bin" ; # checksum -p MK64FN1M0xxx12 -d "MP3-Final_sdcard_fatfs.bin";
	-@echo ' '

.PHONY: all clean dependents main-build post-build

-include ../makefile.targets
