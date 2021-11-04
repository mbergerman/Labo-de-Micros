################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include source/rtos/uCOSIII/src/uCOS-III/Source/subdir.mk
-include source/rtos/uCOSIII/src/uCOS-III/Ports/ARM-Cortex-M4/Generic/GNU/subdir.mk
-include source/rtos/uCOSIII/src/uC-CPU/ARM-Cortex-M4/GNU/subdir.mk
-include source/rtos/uCOSIII/src/uC-CPU/subdir.mk
-include source/rtos/uCOSIII/BSP/subdir.mk
-include source/subdir.mk
-include drivers/subdir.mk
-include SDK/startup/subdir.mk
-include SDK/CMSIS/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: TP4_G1.axf

# Tool invocations
TP4_G1.axf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: MCU Linker'
	arm-none-eabi-gcc -nostdlib -Xlinker -Map="TP4_G1.map" -Xlinker --gc-sections -Xlinker -print-memory-usage -Xlinker --sort-section=alignment -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -T "TP4_G1_Debug.ld" -o "TP4_G1.axf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '
	$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) $(EXECUTABLES)$(OBJS)$(C_DEPS) TP4_G1.axf
	-@echo ' '

post-build:
	-@echo 'Performing post-build steps'
	-arm-none-eabi-size "TP4_G1.axf"; # arm-none-eabi-objcopy -v -O binary "TP4_G1.axf" "TP4_G1.bin" ; # checksum -p MK64FN1M0xxx12 -d "TP4_G1.bin";
	-@echo ' '

.PHONY: all clean dependents post-build

-include ../makefile.targets
