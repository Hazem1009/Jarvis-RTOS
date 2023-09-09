################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../JARVIS/CortexMX_OS_Porting.c \
../JARVIS/JARVIS_OS_FIFO.c \
../JARVIS/schedular.c 

OBJS += \
./JARVIS/CortexMX_OS_Porting.o \
./JARVIS/JARVIS_OS_FIFO.o \
./JARVIS/schedular.o 

C_DEPS += \
./JARVIS/CortexMX_OS_Porting.d \
./JARVIS/JARVIS_OS_FIFO.d \
./JARVIS/schedular.d 


# Each subdirectory must supply rules for building sources it contributes
JARVIS/CortexMX_OS_Porting.o: ../JARVIS/CortexMX_OS_Porting.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g -DSTM32 -DSTM32F1 -DSTM32F103C8Tx -DDEBUG -c -I"F:/Learn-IN-Depth/LEARN-IN-DEPTH/Jarvis-RTOS/JARVIS_RTOS_PROJECt/JARVIS/Inc" -I"F:/Learn-IN-Depth/LEARN-IN-DEPTH/Jarvis-RTOS/JARVIS_RTOS_PROJECt/CMSIS_V5" -I../Inc -O0 -ffunction-sections -fdata-sections -Wall -gdwarf-2 -fstack-usage -MMD -MP -MF"JARVIS/CortexMX_OS_Porting.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
JARVIS/JARVIS_OS_FIFO.o: ../JARVIS/JARVIS_OS_FIFO.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g -DSTM32 -DSTM32F1 -DSTM32F103C8Tx -DDEBUG -c -I"F:/Learn-IN-Depth/LEARN-IN-DEPTH/Jarvis-RTOS/JARVIS_RTOS_PROJECt/JARVIS/Inc" -I"F:/Learn-IN-Depth/LEARN-IN-DEPTH/Jarvis-RTOS/JARVIS_RTOS_PROJECt/CMSIS_V5" -I../Inc -O0 -ffunction-sections -fdata-sections -Wall -gdwarf-2 -fstack-usage -MMD -MP -MF"JARVIS/JARVIS_OS_FIFO.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
JARVIS/schedular.o: ../JARVIS/schedular.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g -DSTM32 -DSTM32F1 -DSTM32F103C8Tx -DDEBUG -c -I"F:/Learn-IN-Depth/LEARN-IN-DEPTH/Jarvis-RTOS/JARVIS_RTOS_PROJECt/JARVIS/Inc" -I"F:/Learn-IN-Depth/LEARN-IN-DEPTH/Jarvis-RTOS/JARVIS_RTOS_PROJECt/CMSIS_V5" -I../Inc -O0 -ffunction-sections -fdata-sections -Wall -gdwarf-2 -fstack-usage -MMD -MP -MF"JARVIS/schedular.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

