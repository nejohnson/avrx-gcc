/*
   avrxc_queuepid.c - Queue a PID on the run queue

   Copyright (c)2024    Neil Johnson (neil@njohnson.co.uk)

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

#include "avrx.h"
#include "avrxcore.h"

/*****************************************************************************
 *
 *  FUNCTION
 *      _avrxQueuePid
 *
 *  SYNOPSIS
 *      int8_t _avrxQueuePid(pProcessID pPid)
 *
 *  DESCRIPTION
 *      Takes a PID and inserts it into the run queue.  The run queue is sorted
 *      by priority.  Lower numbers (higher priority) go first.  If there are 
 *      multiple tasks of equal priority, then the new task is appended to the
 *      list of equals (round robin).
 *
 *  RETURNS
 *      -1    : if task is suspended
 *		 0    : if task at top of Run Queue
 *       1..N : position of task in the Run Queue
 *
 *****************************************************************************/

int8_t _avrxQueuePid(pProcessID pPid)
{
    if (pPid->flags & (AVRX_PID_Idle | AVRX_PID_Suspend))
    {
        /* If the PID is idle or about to suspend then just mark it as
        * suspended and return -1.
        */
        pPid->flags |= AVRX_PID_Suspended;
        return -1;
    }
    else
    {
        /* Walk the RunQueue in descending priority to find a place to insert
        * this PID into the queue.
        */
	    pProcessID pCurr, pNext = (pProcessID)(&(AvrXKernelData.RunQueue));
        int8_t counter = -1;

        uint8_t sreg = SREG;
        cli();

        do {
            counter++;
            pCurr = pNext;
            pNext = pCurr->next;
        } while ((pNext != NOPID) && pNext->priority > pPid->priority);

        /* Place found, insert in list, then restore interrutps and return
        * the depth in the RunQueue.
        */
        pCurr->next = pPid;
        pPid->next  = pNext;
        SREG        = sreg;
        return counter;
    }
}

/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/
