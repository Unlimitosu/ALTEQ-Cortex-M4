################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/aes.c \
../Core/Src/atf.c \
../Core/Src/compress.c \
../Core/Src/expand.c \
../Core/Src/field.c \
../Core/Src/main.c \
../Core/Src/matrix.c \
../Core/Src/nistseedexpander.c \
../Core/Src/randombytes.c \
../Core/Src/sign.c \
../Core/Src/stm32f4xx_hal_msp.c \
../Core/Src/stm32f4xx_it.c \
../Core/Src/syscalls.c \
../Core/Src/sysmem.c \
../Core/Src/system_stm32f4xx.c 

OBJS += \
./Core/Src/aes.o \
./Core/Src/atf.o \
./Core/Src/compress.o \
./Core/Src/expand.o \
./Core/Src/field.o \
./Core/Src/main.o \
./Core/Src/matrix.o \
./Core/Src/nistseedexpander.o \
./Core/Src/randombytes.o \
./Core/Src/sign.o \
./Core/Src/stm32f4xx_hal_msp.o \
./Core/Src/stm32f4xx_it.o \
./Core/Src/syscalls.o \
./Core/Src/sysmem.o \
./Core/Src/system_stm32f4xx.o 

C_DEPS += \
./Core/Src/aes.d \
./Core/Src/atf.d \
./Core/Src/compress.d \
./Core/Src/expand.d \
./Core/Src/field.d \
./Core/Src/main.d \
./Core/Src/matrix.d \
./Core/Src/nistseedexpander.d \
./Core/Src/randombytes.d \
./Core/Src/sign.d \
./Core/Src/stm32f4xx_hal_msp.d \
./Core/Src/stm32f4xx_it.d \
./Core/Src/syscalls.d \
./Core/Src/sysmem.d \
./Core/Src/system_stm32f4xx.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/%.o Core/Src/%.su Core/Src/%.cyclo: ../Core/Src/%.c Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F429xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -Ofast -ffunction-sections -fdata-sections -Wall -u _printf_long_long -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src

clean-Core-2f-Src:
	-$(RM) ./Core/Src/aes.cyclo ./Core/Src/aes.d ./Core/Src/aes.o ./Core/Src/aes.su ./Core/Src/atf.cyclo ./Core/Src/atf.d ./Core/Src/atf.o ./Core/Src/atf.su ./Core/Src/compress.cyclo ./Core/Src/compress.d ./Core/Src/compress.o ./Core/Src/compress.su ./Core/Src/expand.cyclo ./Core/Src/expand.d ./Core/Src/expand.o ./Core/Src/expand.su ./Core/Src/field.cyclo ./Core/Src/field.d ./Core/Src/field.o ./Core/Src/field.su ./Core/Src/main.cyclo ./Core/Src/main.d ./Core/Src/main.o ./Core/Src/main.su ./Core/Src/matrix.cyclo ./Core/Src/matrix.d ./Core/Src/matrix.o ./Core/Src/matrix.su ./Core/Src/nistseedexpander.cyclo ./Core/Src/nistseedexpander.d ./Core/Src/nistseedexpander.o ./Core/Src/nistseedexpander.su ./Core/Src/randombytes.cyclo ./Core/Src/randombytes.d ./Core/Src/randombytes.o ./Core/Src/randombytes.su ./Core/Src/sign.cyclo ./Core/Src/sign.d ./Core/Src/sign.o ./Core/Src/sign.su ./Core/Src/stm32f4xx_hal_msp.cyclo ./Core/Src/stm32f4xx_hal_msp.d ./Core/Src/stm32f4xx_hal_msp.o ./Core/Src/stm32f4xx_hal_msp.su ./Core/Src/stm32f4xx_it.cyclo ./Core/Src/stm32f4xx_it.d ./Core/Src/stm32f4xx_it.o ./Core/Src/stm32f4xx_it.su ./Core/Src/syscalls.cyclo ./Core/Src/syscalls.d ./Core/Src/syscalls.o ./Core/Src/syscalls.su ./Core/Src/sysmem.cyclo ./Core/Src/sysmem.d ./Core/Src/sysmem.o ./Core/Src/sysmem.su ./Core/Src/system_stm32f4xx.cyclo ./Core/Src/system_stm32f4xx.d ./Core/Src/system_stm32f4xx.o ./Core/Src/system_stm32f4xx.su

.PHONY: clean-Core-2f-Src

