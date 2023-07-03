/*
	avrx_terminate.S

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

        _MODULE avrx_terminate
		
/*+
; --------------------------------------------------
; AvrXTerminate
;
; Set the IDLE bit so it can not be queued again.  Then attempt to 
; Dequeue from run queue.
; Can not do anything more than that if it is queued on a semaphore...
;
; PASSED:       R25:R24 = Pid to terminate
; RETURNS:
; USES:
; CALLS:
; ASSUMES:
; NOTES:
-*/

        _FUNCTION AvrXTerminate
		
AvrXTerminate:                  ; Entry point for killing other tasks.
        BeginCritical
        rcall   AvrXEnterKernel
        EndCritical

        mov     Zh, p1h
        mov     Zl, p1l
        ldi     Xl, BV(IdleBit) ; Mark task dead
        std     Z+PidState, Xl
        mov     p2h, p1h
        mov     p2l, p1l
        ldi     Zh, hi8(AvrXKernelData+RunQueue)
        ldi     Zl, lo8(AvrXKernelData+RunQueue)
        BeginCritical
        rcall   _RemoveObject           ; Attempt to remove from run queue
        rjmp    _Epilog

        _ENDFUNC AvrXTerminate

