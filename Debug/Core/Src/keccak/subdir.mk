################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/keccak/Keccak-readable-and-compact.c 

OBJS += \
./Core/Src/keccak/Keccak-readable-and-compact.o 

C_DEPS += \
./Core/Src/keccak/Keccak-readable-and-compact.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/keccak/%.o Core/Src/keccak/%.su Core/Src/keccak/%.cyclo: ../Core/Src/keccak/%.c Core/Src/keccak/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F429xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -Ofast -ffunction-sections -fdata-sections -Wall -u _printf_long_long -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-keccak

clean-Core-2f-Src-2f-keccak:
	-$(RM) ./Core/Src/keccak/Keccak-readable-and-compact.cyclo ./Core/Src/keccak/Keccak-readable-and-compact.d ./Core/Src/keccak/Keccak-readable-and-compact.o ./Core/Src/keccak/Keccak-readable-and-compact.su

.PHONY: clean-Core-2f-Src-2f-keccak

