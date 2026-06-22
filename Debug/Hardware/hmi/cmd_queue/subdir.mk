################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Hardware/hmi/cmd_queue/cmd_queue.c 

OBJS += \
./Hardware/hmi/cmd_queue/cmd_queue.o 

C_DEPS += \
./Hardware/hmi/cmd_queue/cmd_queue.d 


# Each subdirectory must supply rules for building sources it contributes
Hardware/hmi/cmd_queue/%.o Hardware/hmi/cmd_queue/%.su Hardware/hmi/cmd_queue/%.cyclo: ../Hardware/hmi/cmd_queue/%.c Hardware/hmi/cmd_queue/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xE -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I"D:/hzdh_work/DH0306-Bohr/DH0306_MASTER/Hardware/sfud/inc" -I"D:/hzdh_work/DH0306-Bohr/DH0306_MASTER/Hardware/hmi/cmd_queue" -I"D:/hzdh_work/DH0306-Bohr/DH0306_MASTER/Hardware/hmi/count_acquisition" -I"D:/hzdh_work/DH0306-Bohr/DH0306_MASTER/Hardware/hmi/hmi_driver" -O0 -ffunction-sections -fdata-sections -Wall -u _printf_float -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Hardware-2f-hmi-2f-cmd_queue

clean-Hardware-2f-hmi-2f-cmd_queue:
	-$(RM) ./Hardware/hmi/cmd_queue/cmd_queue.cyclo ./Hardware/hmi/cmd_queue/cmd_queue.d ./Hardware/hmi/cmd_queue/cmd_queue.o ./Hardware/hmi/cmd_queue/cmd_queue.su

.PHONY: clean-Hardware-2f-hmi-2f-cmd_queue

