################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../drivers/DRV_Board.c \
../drivers/DRV_LEDs.c \
../drivers/DRV_Reader.c \
../drivers/DRV_Stub_Display.c \
../drivers/DRV_Stub_Encoder.c \
../drivers/DRV_Timers.c \
../drivers/PDRV_GPIO.c \
../drivers/PDRV_SysTick.c 

OBJS += \
./drivers/DRV_Board.o \
./drivers/DRV_LEDs.o \
./drivers/DRV_Reader.o \
./drivers/DRV_Stub_Display.o \
./drivers/DRV_Stub_Encoder.o \
./drivers/DRV_Timers.o \
./drivers/PDRV_GPIO.o \
./drivers/PDRV_SysTick.o 

C_DEPS += \
./drivers/DRV_Board.d \
./drivers/DRV_LEDs.d \
./drivers/DRV_Reader.d \
./drivers/DRV_Stub_Display.d \
./drivers/DRV_Stub_Encoder.d \
./drivers/DRV_Timers.d \
./drivers/PDRV_GPIO.d \
./drivers/PDRV_SysTick.d 


# Each subdirectory must supply rules for building sources it contributes
drivers/%.o: ../drivers/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DCPU_MK64FN1M0VLL12 -D__USE_CMSIS -DDEBUG -DFSL_RTOS_BM -DSDK_OS_BAREMETAL -DSDK_DEBUGCONSOLE=0 -I../CMSIS -I../drivers -I../source -I../ -I../SDK/CMSIS -I../SDK/startup -O0 -fno-common -g3 -Wall -c -ffunction-sections -fdata-sections -ffreestanding -fno-builtin -fmerge-constants -fmacro-prefix-map="../$(@D)/"=. -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


