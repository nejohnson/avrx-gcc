/*
	avrx_reschedule.S

	Copyright (C)1998 - 2002 Larry Barello (larry@barello.net)
	Copyright (C)2023        Neil Johnson (neil@njohnson.co.uk)

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
#include        "avrx.inc"

        _MODULE avrx_reschedule.S

;+-------------------------------------------------------------------
;
; void AvrXIntReschedule(void)  --- DELETE --- DELETE --- DELETE --- DELETE --- DELETE 
;
; Yield function: to allow cooperative tasking amoung like priority tasks.
;
; NOTE: Can only be called from AvrX Kernel mode (e.g. inside an interrupt
; handler).  Reschedules the top of the run queue, which, as noted below, may
; not actually be the current running task.
;
        _FUNCTION AvrXIntReschedule

AvrXIntReschedule:
		lds		Zl, AvrXKernelData+RunQueue+NextL	; Grab the top of the run queue
		lds		Zh, AvrXKernelData+RunQueue+NextH
		adiw	Zl, 0
		brne	air1
		ret				; Exit if empty
air1:
		ldd		p1l, Z+NextL		; Point to the next
		ldd		p1h, Z+NextH
		sts		RunQueue+NextL, p1l
		sts		RunQueue+NextH, p1h
		rjmp	_QueuePid

		_ENDFUNC AvrXIntReschedule

;+------------------------------------------------------------------
;
; void AvrXYield(void)
;
; Removes self from run queue and re-queues.  If other tasks of the
; same priority are on the queue, self will queue behind them (round robin)
;
; Note: we remove "Running" which may or may not be the top of the queue
; depending upon whatever other interrupt activity might be going on.  If
; we just removed the top of the run queue, it would work virutally all the
; time, but technically, it could fail to reschedule the correct task (only
; once in a blue moon...)
;
; 09/19/05 - moved BeginCritical from front of routine to current position
; which protects grabbing the running pointer for passing onto QueuePid.
;-

		_FUNCTION AvrXYield

AvrXYield:
        BeginCritical
        rcall   AvrXEnterKernel
        EndCritical

		ldi		Zl, lo8(AvrXKernelData+RunQueue)
		ldi		Zh, hi8(AvrXKernelData+RunQueue)
		BeginCritical
		ldd		p2l, Z+Running+NextL
		ldd		p2h, Z+Running+NextH
		rcall	_RemoveObject		; Can't fail, so don't bother checking
		EndCritical
		mov		p1l, p2l
		mov		p1h, p2h
		rcall	_QueuePid		; requeue ourselves
		rjmp	_Epilog			; jump to the new top of the queue

		_ENDFUNC AvrXYield
