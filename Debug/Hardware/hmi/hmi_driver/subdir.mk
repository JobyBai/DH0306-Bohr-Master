################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Hardware/hmi/hmi_driver/hmi_driver.c 

OBJS += \
./Hardware/hmi/hmi_driver/hmi_driver.o 

C_DEPS += \
./Hardware/hmi/hmi_driver/hmi_driver.d 


# Each subdirectory must supply rules for building sources it contributes
Hardware/hmi/hmi_driver/%.o Hardware/hmi/hmi_driver/%.su Hardware/hmi/hmi_driver/%.cyclo: ../Hardware/hmi/hmi_driver/%.c Hardware/hmi/hmi_driver/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xE -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I"D:/hzdh_work/DH0306-Bohr/DH0306_MASTER/Hardware/sfud/inc" -I"D:/hzdh_work/DH0306-Bohr/DH0306_MASTER/Hardware/hmi/cmd_queue" -I"D:/hzdh_work/DH0306-Bohr/DH0306_MASTER/Hardware/hmi/count_acquisition" -I"D:/hzdh_work/DH0306-Bohr/DH0306_MASTER/Hardware/hmi/hmi_driver" -O0 -ffunction-sections -fdata-sections -Wall -u _printf_float -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Hardware-2f-hmi-2f-hmi_driver

clean-Hardware-2f-hmi-2f-hmi_driver:
	-$(RM) ./Hardware/hmi/hmi_driver/hmi_driver.cyclo ./Hardware/hmi/hmi_driver/hmi_driver.d ./Hardware/hmi/hmi_driver/hmi_driver.o ./Hardware/hmi/hmi_driver/hmi_driver.su

.PHONY: clean-Hardware-2f-hmi-2f-hmi_driver

