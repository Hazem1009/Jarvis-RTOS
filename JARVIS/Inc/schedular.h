/*
 * schedular.h
 *
 *  Created on: Sep 3, 2023
 *      Author: Hazem
 */

#ifndef INC_SCHEDULAR_H_
#define INC_SCHEDULAR_H_


#include "CortexMX_OS_Porting.h"
#include "task.h"
#include "mutex.h"
#include "JARVIS_OS_errors.h"

/*-------------------------------JARVIS APIS-------------------------------*/


/*OS Related APIS*/
OS_Error_t Jarvis_init();
OS_Error_t Jarvis_StartSchedular();

/*Task Related APIS*/

OS_Error_t Jarvis_CreateTask(Task_t* p_task );

OS_Error_t Jarvis_ActivateTask(Task_t* p_task);

OS_Error_t Jarvis_TerminateTask(Task_t* p_task);

OS_Error_t Jarvis_waitTicks(Task_t* p_task,uint32_t TickCount);

uint8_t Jarvis_PriorityGet(Task_t* p_task);

OS_Error_t Jarvis_PrioritySet(Task_t* p_task,uint8_t priority);

OS_Error_t Jarvis_AcquireMutex(Mutex_t* p_mutex,Task_t* p_task);

OS_Error_t Jarvis_ReleaseMutex(Mutex_t* p_mutex,Task_t* p_task);


/*-------------------------------END OF JARVIS APIS-------------------------*/


#endif /* INC_SCHEDULAR_H_ */
