/*
   avrx_starttimer.c - Start a timer

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
#include "avrxkernel.h"


void AvrXStartTimer(pTimerControlBlock pTCB, uint16_t count)
{
    /* Handle the special case of a zero-count timer */
    if (count==0)
    {
        AvrXSetObjectSemaphore(&pTCB->SObj);
	return;
    }

    /* Switch to kernel context and drop the timer queue level */
    _avrxBeginCritical();
    {
        uint8_t *pUserContext;
        AVRXENTERKERNEL(pUserContext);
        _avrxTimerQLevel--;
    }
    _avrxEndCritical();

    /*
     * Now walk the Timer Queue to insert the TCB in count order.
     */
    pTimerControlBlock *ppPrev, *ppNext = &_avrxTimerQueue;    
    do {
        ppPrev = ppNext;
        ppNext = (pTimerControlBlock *)(&((*ppPrev)->SObj.next));
    } while( *ppNext != (pTimerControlBlock)NOPID && (*ppNext)->count < count);
   
    /* If inserting then update the current entry */
    if (*ppNext)
        (*ppNext)->count -= count;
    
    /* Insert the new TCB into the queue */
    (*ppPrev)->SObj.next = (pSystemObject)pTCB;
    *ppPrev            = pTCB;
    (*ppPrev)->SObj.next = (pSystemObject)*ppNext;
    (*ppPrev)->count = count;
    
    /* Do other timer stuff */
    //TimerHandler();
    _Epilog();
}


/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/
