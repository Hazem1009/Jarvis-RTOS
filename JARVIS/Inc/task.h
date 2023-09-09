/*
 * task.h
 *
 *  Created on: Sep 9, 2023
 *      Author: Hazem
 */

#ifndef INC_TASK_H_
#define INC_TASK_H_

typedef enum
{
	TASK_SUSPENDED,

	TASK_INACTIVE,

	TASK_READY,

	TASK_BLOCKED,

	TASK_RUNNING

}TaskState_t;


/*------------------------------------------------------*/
/*Type Definitions for Blocking on Time*/
typedef enum
{
	TASK_NOT_BLOCKED_ON_TIME,
	TASK_BLOCKED_ON_TIME

}isTimeBlocked_t;

typedef struct
{
	uint8_t isBlocked;
	uint32_t TickCount;

}TimingBlock_t;
/*------------------------------------------------------*/

/*Structure Resembling The Task Information in JARVIS OS
 * Not Everything is Set by USER , the Structure has more
 * information set and used by OS for Scheduling
 * */
typedef struct
{
	uint16_t stack_size;  //Stack Size Reserved in Bytes,Set By user

	uint8_t priority;	 //Task Priority Set by User

	char TaskName[20];

	void(*p_task_enrty)(void);	//Task Function set by User

	/*Next attributes are Os specific Task Information*/

	uint32_t S_PSP_Stack; //start of Task Stack in memory

	uint32_t E_PSP_Stack; //End of Task stack in memory

	uint32_t* task_Current_PSP; //current address PSP is pointing to for the task

	TaskState_t task_state; 	//@ref TaskState_t resembles task state which is one of state machine states

	TimingBlock_t timingParamters;

}Task_t;


#endif /* INC_TASK_H_ */
