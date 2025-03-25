/*
   avrx_intsetsemaphore.c - Set a semaphore from within an interrupt

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


void AvrXIntSetSemaphore(pMutex pSem)
{
    /* Even though we're in interrupt space it is still possible for interrupts
     * to be enabled (good practice) so still need to create a critical region.
     */
    _avrxCriticalEnter();

    /* Trivial case of semaphore is at PEND or DONE then mark it DONE
     * and leave. */
    if((*pSem)->next <= AVRX_SEM_DONE)
    {
	(*pSem)->next = AVRX_SEM_DONE;
	_avrxCriticalLeave();
	return;
    }

    /* Otherwise take the first process from the Semaphore and queue it
    * for execution.
    */
    pProcessID pPID = _avrxRemoveNextObject(*pSem);
    _avrxCriticalLeave();
    _avrxQueuePid(pPID);
    return;
}

/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/
