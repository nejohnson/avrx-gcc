/*
    avrxkernel.h - Interfaces of internal kernel routines

    Copyright (c)2023        Neil Johnson (neil@njohnson.co.uk)

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public
    License along with this library; if not, write to the
    Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA  02111-1307, USA.

    http://www.gnu.org/copyleft/lgpl.html
*/

/*****************************************************************************/
#ifndef AVRXCORE_H
#define AVRXCORE_H
/*****************************************************************************/

#include <stdint.h>

#include <avr/interrupt.h>
#include <avr/io.h>

extern struct AvrXKernelData AvrXKernelData;

/*****************************************************************************
 *
 *  FUNCTION
 *      _avrxQueuePid
 *
 *  SYNOPSIS
 *      int8_t _avrxQueuePid(pProcessID pPid)
 *
 *  DESCRIPTION
 *      Insert a task's PID into the Run Queue.
 *
 *  RETURNS
 *      -1    : if task is suspended
 *		 0    : if task at top of Run Queue
 *       1..N : position of task in the Run Queue
 *
 *****************************************************************************/

extern int8_t _avrxQueuePid(pProcessID);

/*****************************************************************************
 *
 *  FUNCTION
 *      _avrxQueuePid
 *
 *  SYNOPSIS
 *      int8_t _avrxQueuePid(pProcessID pPid)
 *
 *  DESCRIPTION
 *      Insert a task's PID into the Run Queue.
 *
 *  RETURNS
 *      -1    : if task is suspended
 *		 0    : if task at top of Run Queue
 *       1..N : position of task in the Run Queue
 *
 *****************************************************************************/

extern void _avrxAppendObject(pProcessID, pProcessID);

/*****************************************************************************
 *
 *  FUNCTION
 *      _avrxRemoveObjectAt
 *
 *  SYNOPSIS
 *      pProcessID _avrxRemoveObjectAt(pProcessID pQueue, pProcessID pObject)
 *
 *  DESCRIPTION
 *      Removes the Object from the Queue.
 *
 *  RETURNS
 *      Next object in queue
 *
 *****************************************************************************/

extern pProcessID _avrxRemoveObjectAt(pProcessID, pProcessID);

/*****************************************************************************
 *
 *  FUNCTION
 *      _avrxRemoveFirstObject
 *
 *  SYNOPSIS
 *      pProcessID _avrxRemoveFirstObject(pProcessID pQueue)
 *
 *  DESCRIPTION
 *      Removes the first Object from the Queue.
 *
 *  RETURNS
 *      Pointer to the next Object in the queue, or NULL if the queue is now
 *      empty.
 *
 *****************************************************************************/

extern pProcessID _avrxRemoveFirstObject(pProcessID);

/*****************************************************************************
 *
 *  FUNCTION
 *      _avrxRemoveObject
 *
 *  SYNOPSIS
 *      pProcessID _avrxRemoveObject(pProcessID pQueue, pProcessID pObject)
 *
 *  DESCRIPTION
 *      Walks the Queue looking for the Object, then removes it.
 *
 *  RETURNS
 *      Pointer to next Object in the Queue, else NULL if not found or the 
 *      queue is now empty.
 *
 *****************************************************************************/

extern pProcessID _avrxRemoveObject(pProcessID, pProcessID);

/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/
#endif /* AVRXCORE_H */
/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/
