/*
   avrx_resume.c - Resume a task

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
 *      AvrXResume
 *
 *  SYNOPSIS
 *      void AvrXResume(pProcessID pPID)
 *
 *  DESCRIPTION
 *      Takes a PID and inserts it into the run queue.
 *
 *      Uses two flags, Suspended and Suspend to determine if a task is
 *      actually suspended, as opposed to just being marked for suspension.
 *      Mark _SUSP to make QueuePid not queue, Mark _SUSPENDED when attempt
 *      to queue is made (e.g. not blocked on a semaphore).  This is because
 *      we cannot "resume" something that is blocked on a semaphore, even though
 *      it is marked for suspension.
 *
 *  RETURNS
 *      Nothing
 *
 *****************************************************************************/

void AvrXResume(pProcessID pPID)
{
   _avrxBeginCritical();
   uint8_t *pUserContext;
   AVRXENTERKERNEL(pUserContext);
   _avrxEndCritical();
   
   /* Clear the suspend flag */
   pPID->flags &= ~AVRX_PID_Suspend;
   
   /* If removed from the run queue, then remove the suspended
   * bit and put it on the queue.
   */
   if(pPID->flags & AVRX_PID_Suspended)
   {
      pPID->flags &= ~AVRX_PID_Suspended;
      _avrxQueuePid(pPID);
   }
   _Epilog();
}

/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/
