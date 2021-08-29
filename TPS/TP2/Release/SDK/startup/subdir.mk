################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../SDK/startup/hardware.c \
../SDK/startup/main.c \
../SDK/startup/semihost_hardfault.c \
../SDK/startup/startup_mk64f12.c 

OBJS += \
./SDK/startup/hardware.o \
./SDK/startup/main.o \
./SDK/startup/semihost_hardfault.o \
./SDK/startup/startup_mk64f12.o 

C_DEPS += \
./SDK/startup/hardware.d \
./SDK/startup/main.d \
./SDK/startup/semihost_hardfault.d \
./SDK/startup/startup_mk64f12.d 


# Each subdirectory must supply rules for building sources it contributes
SDK/startup/%.o: ../SDK/startup/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DCPU_MK64FN1M0VLL12 -D__USE_CMSIS -DNDEBUG -DFSL_RTOS_BM -DSDK_OS_BAREMETAL -I../CMSIS -I../drivers -I../utilities -I../source -I../ -I../SDK/CMSIS -I../SDK/startup -Ic:/nxp/mcuxpressoide_10.2.0_759/ide/tools/redlib/include -Ic:/nxp/mcuxpressoide_10.2.0_759/ide/tools/features/include -I../SDK -I../startup -Os -fno-common -g -Wall -c -ffunction-sections -fdata-sections -ffreestanding -fno-builtin -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


