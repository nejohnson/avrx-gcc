/*
	avrx_message.S

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

		_MODULE avrx_message.S

/*+
; --------------------------------------------------
; void AvrXIntSendMessage(pMessageQueue, pMessageControlBlock)
;
; PASSED:       p1h:p1l = Queue head
;               p2h:p2l = Message
; RETURNS:
; USES:         Z, Y & some others.  See AvrXIntSetObjectSemaphore
; CALLS:
; ASSUMES:      Called within an _Epilog/_Prolog wrapper
; NOTES:        This could be called from interrupt code
;               but you would need to make sure there is
;               only one source of messages for that queue
;               or wrap _appendObject in a critical section.
-*/
		_FUNCTION AvrXIntSendMessage
		
AvrXIntSendMessage:		
        mov     Zh, p1h
        mov     Zl, p1l
        in		tmp2, _SFR_IO_ADDR(SREG)	// Critical section while preserving I
        cli
        rcall   _AppendObject   ; Append the message onto the queue
		out		_SFR_IO_ADDR(SREG), tmp2
        rjmp    AvrXIntSetObjectSemaphore
		
		_ENDFUNC AvrXIntSendMessage
/*+
; --------------------------------------------------
; pMessageControlBlock AvrXWaitMessage(pMessageQueue)
;
; PASSED:       p1h:p1l = Queue head
; RETURNS:      r1h:r1l = Message
; USES:         Z, flags (See _RemoveFirstObject)
; CALLS:
; ASSUMES:      Null terminated list
; NOTES:        AvrXRecvMessage is atomic for append-only queues
;               just push registers to be saved
-*/
		_FUNCTION AvrXWaitMessage

AvrXWaitMessage:		
        mov     Zl, p1l
        mov     Zh, p1h
        BeginCritical
        rcall   _RemoveFirstObject
        EndCritical
        brne    _rm01

        rcall   AvrXWaitObjectSemaphore
        subi    p1l, lo8(2)
        sbci    p1h, hi8(2)             ; p1 buggered by WaitObject
        rjmp    AvrXWaitMessage
_rm01:
        rcall   AvrXResetObjectSemaphore      ; Clear possible _PEND
        mov     p1l, p2l
        mov     p1h, p2h
        ret
		
		_ENDFUNC AvrXWaitMessage

