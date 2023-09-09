/*
 * JARVIS_OS_FIFIO.h
 *
 *  Created on: Sep 3, 2023
 *      Author: Hazem
 */

#ifndef INC_JARVIS_OS_FIFO_H_
#define INC_JARVIS_OS_FIFO_H_


#include <stdint.h>
#include <stdlib.h>
#include "schedular.h"

typedef Task_t* FIFO_Item;
typedef struct
{
    uint32_t size;
    uint32_t count;
    FIFO_Item *head;
    FIFO_Item *tail;
    FIFO_Item *base;
}FIFO_t;

typedef enum
{
    FIFO_NO_ERROR,
    FIFO_FULL,
    FIFO_NOT_FULL,
    FIFO_EMPTY,
    FIFO_NOT_EMPTY,
    FIFO_NULL
}FIFO_Status;


FIFO_Status FIFO_init(FIFO_t* Buffer,FIFO_Item* arr,uint32_t size);

FIFO_Status FIFO_enqueue(FIFO_t* Buffer,FIFO_Item item);

FIFO_Status FIFO_dequeue(FIFO_t* Buffer,FIFO_Item* item);

FIFO_Status FIFO_is_full(FIFO_t* Buffer);

FIFO_Status FIFO_is_empty(FIFO_t* Buffer);

#endif /* INC_JARVIS_OS_FIFO_H_ */
