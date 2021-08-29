################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../source/App-Baliza_IRQ.c \
../source/App-Baliza_SysTick.c \
../source/App-Ej4.c \
../source/App-Ej5.c \
../source/App-Ej6.c \
../source/App-IRQ.c \
../source/App-SysTick.c \
../source/SysTick.c \
../source/gpio.c 

OBJS += \
./source/App-Baliza_IRQ.o \
./source/App-Baliza_SysTick.o \
./source/App-Ej4.o \
./source/App-Ej5.o \
./source/App-Ej6.o \
./source/App-IRQ.o \
./source/App-SysTick.o \
./source/SysTick.o \
./source/gpio.o 

C_DEPS += \
./source/App-Baliza_IRQ.d \
./source/App-Baliza_SysTick.d \
./source/App-Ej4.d \
./source/App-Ej5.d \
./source/App-Ej6.d \
./source/App-IRQ.d \
./source/App-SysTick.d \
./source/SysTick.d \
./source/gpio.d 


# Each subdirectory must supply rules for building sources it contributes
source/%.o: ../source/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DCPU_MK64FN1M0VLL12 -D__USE_CMSIS -DDEBUG -I../source -I../ -I../SDK/CMSIS -I../SDK/startup -O0 -fno-common -g3 -Wall -c -ffunction-sections -fdata-sections -ffreestanding -fno-builtin -fmerge-constants -fmacro-prefix-map="../$(@D)/"=. -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


