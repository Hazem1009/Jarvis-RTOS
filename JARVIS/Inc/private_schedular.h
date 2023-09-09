/*
 * private_schedular.h
 *
 *  Created on: Sep 8, 2023
 *      Author: Hazem
 */

#ifndef INC_PRIVATE_SCHEDULAR_H_
#define INC_PRIVATE_SCHEDULAR_H_
#include "task.h"
#include "JARVIS_OS_FIFO.h"
#include "JARVIS_OS_errors.h"


/* SVC Numbers For Code Readability*/
typedef enum
{
	SVC_ACTIVATE_TASK,	 //Activate Task - 0

	SVC_TERMINATE_TASK,	 //Terminate Task- 1

	SVC_WAIT_TICKS,		//Wait Ticks -2

	SVC_PRIORITY_SET,	//Priority Set -3

	SVC_PRIORITY_GET,	//Priority Get -4


	SVC_ACQUIRE_MUTEX,	//Acquire Mutex -5

	SVC_RELEASE_MUTEX,	//Release Mutex -6
}SVC_ID;



typedef enum
{
	OS_SUSPEND_MODE,
	OS_RUNNING_MODE
}OS_state;

typedef struct
{
	Task_t* tasks_table[100];	//table of all tasks in system regardless their state

	uint32_t S_MSP;
	uint32_t E_MSP;

	uint32_t PSP_task_locator;

	uint16_t number_of_tasks;

	Task_t* current_Task;
	Task_t* next_Task;

	OS_state state;
}OS_Control_t;

void JARVIS_IdleTask(void);

extern uint32_t _estack;
extern uint32_t _eheap;

/*-----------------------------------------------------------*/
			/*Type definition for OS control Block*/

/*OS_control Object*/
OS_Control_t OS_Control;

/*Tasks Ready Queue*/
FIFO_t OS_Ready_Queue;

/*Array Hold by OS Ready Queue*/
Task_t *OS_Ready_Arr[100];

/*Idle Task*/
Task_t IdleTask;



/*PRIVATE USED APIS*/
OS_Error_t CreateTaskStack(Task_t* p_task);

OS_Error_t OS_CreateMainStack();

void Sort_Schedular_Table();

OS_Error_t UpdateReadyQueue();

OS_Error_t UpdateSchedularTable();

void Decide_NextTask();

void Jarvis_UpdateTasksWaitingTime();

void Jarvis_TriggerSVC(SVC_ID id);


#endif /* INC_PRIVATE_SCHEDULAR_H_ */
