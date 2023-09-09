/*
 * JARVIS_OS_FIFIO.c
 *
 *  Created on: Sep 3, 2023
 *      Author: Hazem
 */


#include "JARVIS_OS_FIFO.h"

FIFO_Status FIFO_init(FIFO_t* Buffer,FIFO_Item* arr,uint32_t size)
{
    if(arr == NULL)
    {
        Buffer=NULL;
        Buffer->base=NULL;
        Buffer->head=NULL;
        Buffer->tail=NULL;
        Buffer->count=0;
        Buffer->size=0;
        return FIFO_NULL;
    }
    Buffer->base=arr;
    Buffer->head=arr;
    Buffer->tail=arr;
    Buffer->count=0;
    Buffer->size=size;
    return FIFO_NO_ERROR;
}

FIFO_Status FIFO_enqueue(FIFO_t* Buffer,FIFO_Item item)
{
    if(Buffer==NULL||Buffer->base==NULL||Buffer->head==NULL||Buffer->tail==NULL)
    {
        return FIFO_NULL;
    }
    // check if the FIFO is full
    if(FIFO_is_full(Buffer)==FIFO_FULL)
    {
        //printf("FIFO is full,can't EnFIFO\n");
        return FIFO_FULL;
    }

    //insert the item
     *(Buffer->head)=item;
    //printf("EnFIFOd item is %d\n",*(Buffer->head));

    //increment the count
    Buffer->count++;

    //circulate the head pointer
    if(Buffer->head >= Buffer->base + (Buffer->size-1))
    {
        Buffer->head = Buffer->base;
    }
    else
    {
        Buffer->head++;
    }

    return FIFO_NO_ERROR;
}
FIFO_Status FIFO_dequeue(FIFO_t* Buffer,FIFO_Item* item)
{
    if(Buffer==NULL||Buffer->base==NULL||Buffer->head==NULL||Buffer->tail==NULL)
    {
        return FIFO_NULL;
    }
    if(Buffer->count==0)
    {
        //printf("FIFO is empty\n");
        return FIFO_EMPTY;
    }

    //deFIFO the item
    *item=*(Buffer->tail);
    //printf("DeFIFOd item is %d\n",*item);
    Buffer->count--;
    //circulate the tail pointer
    if(Buffer->tail == Buffer->base + (Buffer->size))
    {
        Buffer->tail = Buffer->base;
    }
    else
    {
        Buffer->tail++;
    }

    return FIFO_NO_ERROR;
}

FIFO_Status FIFO_is_full(FIFO_t* Buffer)
{
    if(Buffer==NULL||Buffer->base==NULL||Buffer->head==NULL||Buffer->tail==NULL)
    {
        return FIFO_NULL;
    }
    if(Buffer->count>=Buffer->size)
    {
        return FIFO_FULL;
    }

    return FIFO_NOT_FULL;
}


FIFO_Status FIFO_is_empty(FIFO_t* Buffer)
{
    if(Buffer==NULL||Buffer->base==NULL||Buffer->head==NULL||Buffer->tail==NULL)
    {
        return FIFO_NULL;
    }
    if(Buffer->count==0)
    {
        return FIFO_EMPTY;
    }

    return FIFO_NOT_EMPTY  ;

}

