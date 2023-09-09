/*
 * schedular.c
 *
 *  Created on: Sep 3, 2023
 *      Author: Hazem Hisham
 */

#include "schedular.h"
#include "string.h"
#include "JARVIS_OS_FIFO.h"
#include "private_schedular.h"


/*-----------------------------------------------------------*/

/*----------------------------------------Idle Task Section----------------------------------------*/


__attribute((weak)) void IdleHook(void)
{
	__asm("nop");
}

void JARVIS_IdleTask(void)
{
	while(1)
	{
		IdleHook();
		__asm("WFE");
	}
}

/*-------------------------------------------------------------------------------------------------*/


/*----------------------------------------RTOS INITIALIZATION---------------------------------------*/

OS_Error_t OS_CreateMainStack()
{
	OS_Error_t error=OS_NO_ERROR;

	OS_Control.S_MSP=(uint32_t)&_estack; // Start of RAM is start of MSP (Decreasing RAM model)

	OS_Control.E_MSP=OS_Control.S_MSP-MainStackSize; //End of RAM

	//If OS Stack Space Exceeded Stack and reached Heap this should trigger an error.
	if(OS_Control.E_MSP <= ((uint32_t) &_eheap))
	{
		error=OS_MSP_EXCEEDED_LIMIT;

		return error;
	}

	//Set Start of first Task location to be after OS Stack Space with a guard of 8 bytes
	OS_Control.PSP_task_locator=OS_Control.E_MSP-8;

	return error;

}

OS_Error_t Jarvis_init()
{
	OS_Error_t error=OS_NO_ERROR;

	/*Update OS Mode to be suspended till User Start it*/
	OS_Control.state=OS_SUSPEND_MODE;

	/*Initialize Stack Space for OS*/
	error=OS_CreateMainStack();

	/*if Error returned by CreateMainStack function indicating that OS Stack space can't be reserved then we cant
	 * carry on with initializing the OS*/

	if(error==OS_MSP_EXCEEDED_LIMIT)
	{
		error=OS_INIT_ERROR;
		return error;
	}

	/*Initialize OS Ready FIFO Up TO 100 TASKS*/
	if(FIFO_init(&OS_Ready_Queue, OS_Ready_Arr, 100)!=FIFO_NO_ERROR)
	{
		error=OS_INIT_ERROR;
		return error;
	}

	/*Initialize and create Idle Task*/

	IdleTask.priority=255; //should have least priority in order not to preempt any other task

	strcpy(IdleTask.TaskName,"Idle_Task"); //Initialize Task name

	IdleTask.p_task_enrty=JARVIS_IdleTask;

	IdleTask.stack_size=300;

	/*Create Idle Task*/
	error=Jarvis_CreateTask(&IdleTask);

	return error;

}


/*----------------------------------------Task Creation Section----------------------------------------*/


OS_Error_t CreateTaskStack(Task_t* p_task)
{
	OS_Error_t error=OS_NO_ERROR;

	if(p_task==NULL)
	{
		error=OS_TASK_NOT_CREATED;

		return error;
	}


	p_task->S_PSP_Stack=OS_Control.PSP_task_locator;

	p_task->task_Current_PSP=(uint32_t*)p_task->S_PSP_Stack;

	p_task->E_PSP_Stack=(p_task->S_PSP_Stack)-p_task->stack_size;

	if((p_task->E_PSP_Stack)<=((uint32_t) &_eheap))
	{
		return OS_PSP_EXCEEDED_LIMIT;
	}

	OS_Control.PSP_task_locator=(p_task->E_PSP_Stack)-8;

	/*Save Task Stack Frame as if it was interrupted before so that the handler mode would restore
	 * Full Context of The Task.
	 * */

	/******
	 *XPSR 				High Address
	 *PC
	 *LR
	 *R12
	 * R3
	 * R2
	 * R1
	 * R0
	 * R5
	 * ...
	 * R11				Low Address
	 * */

	/*XPSR*/
	(p_task->task_Current_PSP)--;
	*(p_task->task_Current_PSP)=(1<<24); //Should Store XPSR with T bit activated for thumb mode otherwise a fault is generated

	/*PC*/
	(p_task->task_Current_PSP)--;
	*(p_task->task_Current_PSP)=(uint32_t)p_task->p_task_enrty;

	/*LR*/
	(p_task->task_Current_PSP)--;
	*(p_task->task_Current_PSP)=(uint32_t)0xFFFFFFFD; //EXEC_RETURN : Return To Thread mode with PSP

	//Initialize Other 13 General Registers with 0
	for(int i=0;i<13;i++)
	{
		(p_task->task_Current_PSP)--;
		*(p_task->task_Current_PSP)=0;
	}

	return error;

}

OS_Error_t Jarvis_CreateTask(Task_t* p_task )
{
	OS_Error_t error=OS_NO_ERROR;

	if(p_task==NULL)
	{
		error=OS_TASK_NOT_CREATED;

		return error;
	}

	/*Task Stack Initialization */
	error=CreateTaskStack(p_task);

	/*Error Propagation*/
	if(error==OS_PSP_EXCEEDED_LIMIT)
	{
		error=OS_TASK_NOT_CREATED;
		return error;

	}

	/*Initialize Task State*/
	p_task->task_state=TASK_INACTIVE;

	//Check If Task List is full
	if(OS_Control.number_of_tasks >100)
	{
		error=OS_TASK_NOT_CREATED;
		return error;
	}
	/*Update Task List with newly Created Task*/
	OS_Control.tasks_table[OS_Control.number_of_tasks]=p_task;
	OS_Control.number_of_tasks++;


	/*Initialize Time Blockage Parameters*/
	p_task->timingParamters.isBlocked=0;
	p_task->timingParamters.TickCount=0;
	return error;
}
/*-------------------------------------------------------------------------------------------------*/



/*--------------------------------Schedular Related Functions--------------------------------------*/

void Sort_Schedular_Table()
{
	/*Sort Schedular Table with Bubble Sort Algorithm*/
	Task_t* temp=NULL;
	for(uint8_t i=0;i<OS_Control.number_of_tasks-1;i++)
	{
		for(uint8_t j=0;j<OS_Control.number_of_tasks-i-1;j++)
		{
			if(OS_Control.tasks_table[j]->priority > OS_Control.tasks_table[j+1]->priority)
			{
				temp=OS_Control.tasks_table[j];
				OS_Control.tasks_table[j]=OS_Control.tasks_table[j+1];
				OS_Control.tasks_table[j+1]=temp;
			}
		}
	}

}

OS_Error_t UpdateReadyQueue()
{
	Task_t* current_task=NULL;
	Task_t* next_task=NULL;
	Task_t* temp=NULL;

	OS_Error_t error=OS_NO_ERROR;

	/*Empty Ready Queue*/
	while (FIFO_dequeue(&OS_Ready_Queue,&temp)!=FIFO_EMPTY)
	{

	}
	uint8_t HigherPrio_Ready=255;
	/*Update Ready Queue with all Ready Highest Prio Tasks*/
	for(uint8_t i=0;i<OS_Control.number_of_tasks;i++)
	{
		/*Note That Schedular Table has all Task in all states (Blocked,Ready,INACTIVE,....)*/
		if(OS_Control.tasks_table[i]->task_state==TASK_READY||OS_Control.tasks_table[i]->task_state==TASK_RUNNING)
		{
			current_task=OS_Control.tasks_table[i];
			next_task=OS_Control.tasks_table[i+1];

			/*Case We Reached Highest Priority(lowest value) for Ready Tasks*/
			if(next_task==NULL||(current_task->priority<next_task->priority && current_task->priority <= HigherPrio_Ready))
			{
				error=FIFO_enqueue(&OS_Ready_Queue,current_task);
				break;
			}
			else if(current_task->priority==next_task->priority &&current_task->priority <= HigherPrio_Ready)
			{
				HigherPrio_Ready=current_task->priority;  //useful for case 4 3 3 1 (prio) where Index 2 is suspended we would continue to 1 and enqueue it and that's wrong
				if(current_task->task_state!=TASK_RUNNING)
					error=FIFO_enqueue(&OS_Ready_Queue,current_task);
			}
		}
	}

	return error;
}

OS_Error_t UpdateSchedularTable()
{
	OS_Error_t error=OS_NO_ERROR;

	/* Sort Schedular Table with Sorting Algorithm Supported*/
	Sort_Schedular_Table();

	/*Update Ready Queue*/
	UpdateReadyQueue();

	return error;
}

void Decide_NextTask()
{
	Task_t* current_task=OS_Control.current_Task;

	Task_t* next_task=NULL;

	/*Check if Ready Queue is Empty*/
	if(FIFO_is_empty(&OS_Ready_Queue)==FIFO_EMPTY)
	{
		current_task->task_state=TASK_RUNNING;
		/*No ready Tasks re-add currently running Task into Ready Queue*/
		FIFO_enqueue(&OS_Ready_Queue,current_task);
		/*Set Next Task to be Current running task*/
		next_task=current_task;
		OS_Control.next_Task=next_task;
	}
	else
	{
		/*Get Next Task from Ready Queue*/
		FIFO_dequeue(&OS_Ready_Queue,&next_task);

		/*Check if Next Task is same priority as Current Task*/
		if(next_task!=NULL&& next_task->priority==current_task->priority)
		{
			/*Re-add Current Task to Ready Queue*/
			FIFO_enqueue(&OS_Ready_Queue,current_task);

		}
		if(current_task->task_state==TASK_RUNNING)
			current_task->task_state=TASK_READY; //change state of current task to ready
		next_task->task_state=TASK_RUNNING;	//change state of next task to running

		/*Set Next Task to be Current running task*/
		OS_Control.next_Task=next_task;

	}


}

OS_Error_t Jarvis_StartSchedular()
{
	OS_Control.state=OS_RUNNING_MODE;

	/*Update State of Idle task*/
	IdleTask.task_state=TASK_READY;

	/*Update Schedular Table*/
	UpdateSchedularTable();

	/*Decide What Next*/
	Decide_NextTask();

	/*start ticker*/
	StartOSTicker();

	/*Switch TO PSP & load it with IDLE TASK PSP*/
	OS_SWITCH_SP_TO_PSP();


	Trigger_PendSV();

    return OS_NO_ERROR;
}

void Jarvis_UpdateTasksWaitingTime()
{
	Task_t* pTask;
	for(int i=0;i<OS_Control.number_of_tasks;i++)
	{
		pTask=OS_Control.tasks_table[i];
		if(pTask->task_state==TASK_BLOCKED)
		{
			if(pTask->timingParamters.isBlocked==TASK_BLOCKED_ON_TIME)
			{
				pTask->timingParamters.TickCount--;

				if(pTask->timingParamters.TickCount==0)
				{
					pTask->timingParamters.isBlocked=TASK_NOT_BLOCKED_ON_TIME;

					pTask->task_state=TASK_READY;

				}
			}
		}
	}
}


/*-------------------------------------------------------------------------------------------------*/


/*----------------------------------------JARVIS USER APIS----------------------------------------*/

OS_Error_t Jarvis_ActivateTask(Task_t* p_task)
{
	OS_Error_t error=OS_NO_ERROR;

	if(p_task==NULL)
	{
		error=OS_TASK_NOT_CREATED;

		return error;
	}

	if(p_task->task_state==TASK_INACTIVE)
	{
		//Set task to be in ready state to be scheduled
		p_task->task_state=TASK_READY;

		Jarvis_TriggerSVC(SVC_ACTIVATE_TASK);
	}
	//In order not to call ActivateTask on Currently running/ready or Blocked TASK
	else
	{
		error=OS_TASK_ACTIVATION_ERROR;
	}

	return error;
}

OS_Error_t Jarvis_TerminateTask(Task_t* p_task)
{

	OS_Error_t error=OS_NO_ERROR;

	if(p_task==NULL)
	{
		error=OS_TASK_NOT_CREATED;

		return error;
	}

	//To indicate Task is already in Inacitve state if API is called on inactive Task
	if(p_task->task_state==TASK_INACTIVE)
	{
		error=OS_TASK_ALREADY_INACTIVE;
	}

	else
	{
		//Set task to be in INACTIVE state to be scheduled
		p_task->task_state=TASK_INACTIVE;

		Jarvis_TriggerSVC(SVC_TERMINATE_TASK);
	}

	return error;
}


OS_Error_t Jarvis_waitTicks(Task_t* p_task,uint32_t TickCount)
{
	OS_Error_t error=OS_NO_ERROR;

	if(p_task==NULL)
	{
		error=OS_TASK_NOT_CREATED;

		return error;
	}

	if(p_task->task_state==TASK_RUNNING)
	{
		p_task->task_state=TASK_BLOCKED;

		p_task->timingParamters.isBlocked=TASK_BLOCKED_ON_TIME;

		p_task->timingParamters.TickCount=TickCount;

		Jarvis_TriggerSVC(SVC_WAIT_TICKS);

	}
	else
	{
		error=OS_TIME_BLOCKING_IN_NON_RUNNING_STATE;
	}
	return error;
}

uint8_t Jarvis_PriorityGet(Task_t* p_task)
{
	if(p_task==NULL)
	{
		return 0;
	}
	return p_task->priority;

}

OS_Error_t Jarvis_PrioritySet(Task_t* p_task,uint8_t priority)
{
	OS_Error_t error=OS_NO_ERROR;

	if(p_task==NULL)
	{
		error=OS_TASK_NOT_CREATED;
	}

	else
	{
		p_task->priority=priority;

		Jarvis_TriggerSVC(SVC_PRIORITY_SET);
	}

	return error;
}

OS_Error_t Jarvis_AcquireMutex(Mutex_t* p_mutex,Task_t* p_task)
{
	OS_Error_t error=OS_NO_ERROR;


	if(p_task==NULL)
	{
		error=OS_TASK_NOT_CREATED;
		return error;
	}

	if(p_mutex==NULL)
	{
		error=OS_MUTEX_NOT_CREATED;
		return error;
	}

	/*Case NO TASK IS HOLDING THIS MUTEX*/
	if(p_mutex->isAvailable==MUTEX_AVAILABLE)
	{
		//save Priority
		p_mutex->PriorityBeforeInheritance=p_task->priority;

		//Hold the mutex for requesting p_task if no one is holding and No one is blocked on it
		p_mutex->holder=p_task;

		p_mutex->isAvailable=MUTEX_NOT_AVAILABLE;
	}
	/*Case Mutex is held by some Task*/
	else
	{
		/*Priority Inheritance If Currently Requesting Task prio is higher than the one holding the mutex*/
		if(p_task->priority < p_mutex->holder->priority)
		{
			/*Block the Higher Prio task requesting
			 * Assign its prio to The Task Holding the Mutex
			 * Add it to the list of blocked tasks
			 * Call SVC For updating Schedular Table and Releasing all held mutex by this task
			 * */
			p_task->task_state=TASK_BLOCKED;

			p_mutex->holder->priority=p_task->priority;

			p_mutex->BlockedTasks[p_mutex->BlockedTasksCount]=p_task;

			(p_mutex->BlockedTasksCount)++;

			Jarvis_TriggerSVC(SVC_ACQUIRE_MUTEX);

		}
		else
		{
			/* Change State of The Task
			 * Add it to the list of blocked tasks
			 * Call SVC For updating Schedular Table and Releasing all hold mutex by this task*/
			p_task->task_state=TASK_BLOCKED;

			p_mutex->BlockedTasks[p_mutex->BlockedTasksCount]=p_task;

			(p_mutex->BlockedTasksCount)++;

			Jarvis_TriggerSVC(SVC_ACQUIRE_MUTEX);
		}
	}
	return error;

}

OS_Error_t Jarvis_ReleaseMutex(Mutex_t* p_mutex,Task_t* p_task)
{
	OS_Error_t error=OS_NO_ERROR;
	uint8_t HighestPrioIndex=0;

	if(p_task==NULL)
	{
		error=OS_TASK_NOT_CREATED;
		return error;
	}

	if(p_mutex==NULL)
	{
		error=OS_MUTEX_NOT_CREATED;
		return error;
	}

	/*case Current Mutex Holding Task is not the same as the one attempting to release */
	if(p_task!=p_mutex->holder)
	{
		error=OS_MUTEX_NOT_HELD_BY_THIS_TASK;
		return error;
	}

	if(p_mutex->isAvailable==MUTEX_AVAILABLE)
	{
		error=OS_MUTEX_ALREADY_AVAIALABLE_CANT_RELEASE;
		return error;
	}

	Task_t * holder=p_mutex->holder;

	/*Case Priority Inheritance happened- Priorirty Inheritance minimizes the effect of priority
	 * Inversion but doesn't Remove it!*/
	if(holder->priority!=p_mutex->PriorityBeforeInheritance)
	{
		holder->priority=p_mutex->PriorityBeforeInheritance;

		p_mutex->PriorityBeforeInheritance=INVALID_PREVIOUS_PRIORITY;
	}

	/*first Case No one is blocked on this mutex*/
	if(p_mutex->BlockedTasksCount==0)
	{
		p_mutex->isAvailable=MUTEX_AVAILABLE;

		p_mutex->holder=NULL;

		/*TODO: Remove This mutex from held mutex list by this Task*/

		return error;
	}

	/*2nd Case There are Blocked Tasks on this mutex*/
	else
	{
		/*Fetch the highest Task in priority waiting on this mutex
		 * if there is equal highest prio tasks , choose the one waiting for the longest
		 * */
		for(uint8_t i=0;i<(p_mutex->BlockedTasksCount)-1;i++)
		{
			if(p_mutex->BlockedTasks[i]->priority <= p_mutex->BlockedTasks[i+1]->priority)
			{
				HighestPrioIndex=i;
			}
			else
			{
				HighestPrioIndex=i+1;
			}

		}
		Task_t* nextHolder=p_mutex->BlockedTasks[HighestPrioIndex];

		nextHolder->task_state=TASK_READY;

		p_mutex->PriorityBeforeInheritance=nextHolder->priority;

		/*TODO: Remove This mutex from held mutex list by this Task*/

		p_mutex->holder=nextHolder;

		for(uint8_t i=HighestPrioIndex;i<(p_mutex->BlockedTasksCount)-1;i++)
		{
			p_mutex->BlockedTasks[i]=p_mutex->BlockedTasks[i+1];
		}

		p_mutex->BlockedTasksCount--;

		Jarvis_TriggerSVC(SVC_RELEASE_MUTEX);
	}

	return error;

}

/*-------------------------------------------------------------------------------------------------*/



/*----------------------------------------SVCs  Section----------------------------------------*/

void Jarvis_TriggerSVC(SVC_ID id)
{
	switch(id)
	{

	case SVC_ACTIVATE_TASK:
		__asm("SVC #0x00");
		break;

	case SVC_TERMINATE_TASK:
		__asm("SVC #0x01");
		break;

	case SVC_WAIT_TICKS:
		__asm("SVC #0x02");
		break;

	case SVC_PRIORITY_SET:
		__asm("SVC #0x03");
		break;

	case SVC_PRIORITY_GET:
		__asm("SVC #0x04");
		break;

	case SVC_ACQUIRE_MUTEX:
		__asm("SVC #0x05");
		break;

	case SVC_RELEASE_MUTEX:
		__asm("SVC #0x06");
		break;

	}
}

void SVC_SW_HANDLER(uint32_t* StackFrame)
{
	// R0 R1 R2 R3 R12 LR PC(what we want to reach is in previous thumb instruction from this pc) XPSR
	uint8_t svcID= *((uint8_t*)(((uint8_t*)StackFrame[6]) -2));

	switch(svcID)
	{
		case SVC_ACTIVATE_TASK:
		case SVC_TERMINATE_TASK:
			//Update Schedular Table and within it Ready Queue
			if(OS_Control.state==OS_RUNNING_MODE)
			{
				UpdateSchedularTable();

				//Decide Next Task for OS
				Decide_NextTask();

				Trigger_PendSV();
			}
			break;

		case SVC_WAIT_TICKS:
			UpdateSchedularTable();
			Decide_NextTask();
			if(OS_Control.next_Task!=OS_Control.current_Task && OS_Control.state==OS_RUNNING_MODE)
			{
				Trigger_PendSV();
			}
			break;

		case SVC_PRIORITY_SET:
			UpdateSchedularTable();
			Decide_NextTask();
			if(OS_Control.next_Task!=OS_Control.current_Task && OS_Control.state==OS_RUNNING_MODE)
			{
				Trigger_PendSV();
			}
			break;

		case SVC_PRIORITY_GET:
			break;

		case SVC_ACQUIRE_MUTEX:

			/*if Task Couldn't Acquire the mutex*/
			if(OS_Control.current_Task->task_state==TASK_BLOCKED)
			{
				/*	Release All mutex hold by this Task to avoid Deadlock
				 * 	Update Schedular Table
				 * 	Decide Next Task
				 * 	Trigger PENDSV
				 * */
				UpdateSchedularTable();
				Decide_NextTask();
				Trigger_PendSV();
			}
			else
			{
				/*DO NOTHING:TASK already Acquired The mutex*/
			}
			break;

		case SVC_RELEASE_MUTEX:
			UpdateSchedularTable();
			Decide_NextTask();
			if(OS_Control.current_Task!=OS_Control.next_Task)
			{
				Trigger_PendSV();
			}
			break;

		}
}

/*-------------------------------------------------------------------------------------------------*/


/*----------------------------------------PENDSV HANDLER----------------------------------------*/

__attribute((naked)) void PendSV_Handler(void)
{
	/*Switch ACCESS LEVEL*/
		OS_SWITCH_TO_UNPRIVILEGE();
	/*Push Context of Current Task*/

	if(OS_Control.current_Task!=NULL)
	{

		OS_GET_PSP((uint32_t)OS_Control.current_Task->task_Current_PSP);
		/*PUSH R4:R11*/


		OS_Control.current_Task->task_Current_PSP-- ;
		__asm volatile("mov %0,r4 " : "=r" (*(OS_Control.current_Task->task_Current_PSP)));

		OS_Control.current_Task->task_Current_PSP-- ;
		__asm volatile("mov %0,r5 " : "=r" (*(OS_Control.current_Task->task_Current_PSP)));

		OS_Control.current_Task->task_Current_PSP-- ;
		__asm volatile("mov %0,r6 " : "=r" (*(OS_Control.current_Task->task_Current_PSP)));

		OS_Control.current_Task->task_Current_PSP-- ;
		__asm volatile("mov %0,r7 " : "=r" (*(OS_Control.current_Task->task_Current_PSP)));

		OS_Control.current_Task->task_Current_PSP-- ;
		__asm volatile("mov %0,r8 " : "=r" (*(OS_Control.current_Task->task_Current_PSP)));

		OS_Control.current_Task->task_Current_PSP-- ;
		__asm volatile("mov %0,r9 " : "=r" (*(OS_Control.current_Task->task_Current_PSP)));

		OS_Control.current_Task->task_Current_PSP-- ;
		__asm volatile("mov %0,r10 " : "=r" (*(OS_Control.current_Task->task_Current_PSP)));

		OS_Control.current_Task->task_Current_PSP-- ;
		__asm volatile("mov %0,r11 " : "=r" (*(OS_Control.current_Task->task_Current_PSP)));
	}


	/*Restore Context of next task*/
	if(OS_Control.next_Task!=NULL)
	{
		OS_Control.current_Task=OS_Control.next_Task;

		OS_Control.next_Task=NULL;

		/*Restore R11:R5*/
		__asm volatile("mov r11,%0 " : :"r" (*(OS_Control.current_Task->task_Current_PSP)));
		OS_Control.current_Task->task_Current_PSP++ ;

		__asm volatile("mov r10,%0 " : :"r" (*(OS_Control.current_Task->task_Current_PSP)));
		OS_Control.current_Task->task_Current_PSP++ ;

		__asm volatile("mov r9,%0 " : :"r" (*(OS_Control.current_Task->task_Current_PSP)));
		OS_Control.current_Task->task_Current_PSP++ ;

		__asm volatile("mov r8,%0 " : :"r" (*(OS_Control.current_Task->task_Current_PSP)));
		OS_Control.current_Task->task_Current_PSP++ ;

		__asm volatile("mov r7,%0 " : :"r" (*(OS_Control.current_Task->task_Current_PSP)));
		OS_Control.current_Task->task_Current_PSP++ ;

		__asm volatile("mov r6,%0 " : :"r" (*(OS_Control.current_Task->task_Current_PSP)));
		OS_Control.current_Task->task_Current_PSP++ ;

		__asm volatile("mov r5,%0 " : :"r" (*(OS_Control.current_Task->task_Current_PSP)));
		/*This line we may need to increment Current PSP*/
		OS_Control.current_Task->task_Current_PSP++ ;
		__asm volatile("mov r4,%0 " : :"r" (*(OS_Control.current_Task->task_Current_PSP)));
		OS_Control.current_Task->task_Current_PSP++ ;

		OS_SET_PSP((uint32_t)OS_Control.current_Task->task_Current_PSP);

		__asm volatile("BX LR");


	}



}

/*-------------------------------------------------------------------------------------------------*/
