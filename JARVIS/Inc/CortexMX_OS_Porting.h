/*
 * CortexMX_OS_Porting.h
 *
 *  Created on: Sep 3, 2023
 *      Author: Hazem
 */

#ifndef INC_CORTEXMX_OS_PORTING_H_
#define INC_CORTEXMX_OS_PORTING_H_

#include "ARMCM3.h"

extern  uint32_t _estack;
extern  uint32_t _eheap;
#define MainStackSize 3072

typedef enum
{
	E_NOT_OK,
	E_OK
}PORTING_ERROR_t;


//Macro To switch to unprivileged mode
#define OS_SWITCH_TO_UNPRIVILEGE() __asm(" MRS R0,CONTROL \n\t ORR R0,#0x1 \n\t MSR CONTROL,R0")

//Macro To switch to privileged mode
#define OS_SWITCH_TO_PRIVILEGE() __asm(" MRS R0,CONTROL \n\t lsr R0,#0x1 \n\t lsl R0,#0x1 \n\t MSR CONTROL,R0")

//Macro To make SP Shadow PSP
#define OS_SWITCH_SP_TO_PSP() 	__asm("MRS R0,CONTROL \n\t MOV R1,#0x2 \n\t ORR R0,R1 \n\t MSR CONTROL,R0")

//Macro To make SP Shadow MSP
#define OS_SWITCH_SP_TO_MSP() 	__asm("MRS R0,CONTROL \n\t MOV R1,#0x5 \n\t AND R0,R1 \n\t MSR CONTROL,R0")

//MACRO TO GET PSP
#define OS_GET_PSP(add)			__asm("MRS R0,PSP \n\t MOV %0,R0" : "=r" (add))

//MACRO TO SET PSP
#define OS_SET_PSP(add)			__asm(" MOV R0,%0 \n\t MSR PSP,R0" : :"r" (add))


PORTING_ERROR_t HW_init(void);
void Trigger_PendSV(void);
PORTING_ERROR_t StartOSTicker();


#endif /* INC_CORTEXMX_OS_PORTING_H_ */
