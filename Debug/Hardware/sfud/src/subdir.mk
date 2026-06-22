################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Hardware/sfud/src/sfud.c \
../Hardware/sfud/src/sfud_sfdp.c 

OBJS += \
./Hardware/sfud/src/sfud.o \
./Hardware/sfud/src/sfud_sfdp.o 

C_DEPS += \
./Hardware/sfud/src/sfud.d \
./Hardware/sfud/src/sfud_sfdp.d 


# Each subdirectory must supply rules for building sources it contributes
Hardware/sfud/src/%.o Hardware/sfud/src/%.su Hardware/sfud/src/%.cyclo: ../Hardware/sfud/src/%.c Hardware/sfud/src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xE -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I"D:/hzdh_work/DH0306-Bohr/DH0306_MASTER/Hardware/sfud/inc" -I"D:/hzdh_work/DH0306-Bohr/DH0306_MASTER/Hardware/hmi/cmd_queue" -I"D:/hzdh_work/DH0306-Bohr/DH0306_MASTER/Hardware/hmi/count_acquisition" -I"D:/hzdh_work/DH0306-Bohr/DH0306_MASTER/Hardware/hmi/hmi_driver" -O0 -ffunction-sections -fdata-sections -Wall -u _printf_float -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Hardware-2f-sfud-2f-src

clean-Hardware-2f-sfud-2f-src:
	-$(RM) ./Hardware/sfud/src/sfud.cyclo ./Hardware/sfud/src/sfud.d ./Hardware/sfud/src/sfud.o ./Hardware/sfud/src/sfud.su ./Hardware/sfud/src/sfud_sfdp.cyclo ./Hardware/sfud/src/sfud_sfdp.d ./Hardware/sfud/src/sfud_sfdp.o ./Hardware/sfud/src/sfud_sfdp.su

.PHONY: clean-Hardware-2f-sfud-2f-src

