/*
   avrx_suspend.c - Suspend a task

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
 *      AvrXSuspend
 *
 *  SYNOPSIS
 *      void AvrXSuspend(pProcessID pPID)
 *
 *  DESCRIPTION
 *      Mark a pid for suspension and attempt to remove from the run queue.
 *      If removed from the run queue, then marks suspended.  _avrxQueuePid 
 *      will take care of this for pids queued elsewhere when they become
 *      eligible for running.
 *
 *  RETURNS
 *      Nothing
 *
 *****************************************************************************/

void AvrXSuspend(pProcessID pPID)
{
   _avrxBeginCritical();
   uint8_t *pUserContext;
   AVRXENTERKERNEL(pUserContext);
   _avrxEndCritical();

   /* Mark this PID is going into Suspend */
   pPID->flags |= AVRX_PID_Suspend;

   _avrxBeginCritical();
   if(_avrxRemoveObject(AvrXKernelData.RunQueue, pPID))
   {
      /* PID was found in, and removed from, the run queue so mark it as
      * Suspended.
      */
      pPID->flags |= AVRX_PID_Suspended;
   }
   _Epilog();
}

/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/
