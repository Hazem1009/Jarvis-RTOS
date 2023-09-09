/*
 * mutex.h
 *
 *  Created on: Sep 9, 2023
 *      Author: Hazem
 */

#ifndef INC_MUTEX_H_
#define INC_MUTEX_H_

#include "task.h"


#define INVALID_PREVIOUS_PRIORITY  0xFFFF

typedef enum
{
	MUTEX_AVAILABLE,

	MUTEX_NOT_AVAILABLE
}Mutex_status;

typedef struct
{
	Mutex_status isAvailable;	//flag

	Task_t* holder;				//Current Task Holding The mutex

	Task_t* BlockedTasks[99];  //Array of Tasks blocked on this mutex

	uint8_t BlockedTasksCount; //Count of Tasks Blocked on this mutex

	uint16_t PriorityBeforeInheritance; //In case Of priority inheritance
}Mutex_t;



#endif /* INC_MUTEX_H_ */
