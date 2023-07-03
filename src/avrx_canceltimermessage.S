/*
	avrx_canceltimermessage.S

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

		_MODULE avrx_canceltimermessage.S

/*+
; -----------------------------------------------
; pTimerControlBlock
; AvrXCancelTimerMessage(pTimerControlBlock, pMessageQueue) CONST
;
; Passed:       R25:R24 = pTimerControlBlock
;               R23:R22 = pMessageQueue
; Returns:      R25:R24 = Pointer to removed timer, or 0 if failure
; Uses:
; Stack:        
; Note: walk list looking for TCB.  If found, and not at
;       end of list, then add count to Next tcb. Else, tries
;       to remove from message queue.
;
-*/
		_FUNCTION AvrXCancelTimerMessage

AvrXCancelTimerMessage:		
        BeginCritical
        rcall   AvrXEnterKernel
        EndCritical

        mov     p2l, p1l
        mov     p2h, p1h
        ldi     Zl, lo8(_TimerQueue)
        ldi     Zh, hi8(_TimerQueue)

        BeginCritical
        rcall   _RemoveObject   ; Z = next item, R23:R22 = obj.
        subi    tmp0, lo8(0)
        sbci    tmp1, hi8(0)    ; Test if in timer queue
        breq    actm00          ; No, remove from message queue

        mov     Yl, p2l         ; Y is restored in _Epilog
        mov     Yh, p2h
        ldd     p2l, Y+TcbCount+NextL
        ldd     p2h, Y+TcbCount+NextH
        ldd     r1h, Z+TcbCount+NextH ; Next
        ldd     r1l, Z+TcbCount+NextL
        add     r1l, p2l
        adc     r1h, p2h
        std     Z+TcbCount+NextL, r1l
        std     Z+TcbCount+NextH, r1h
        rjmp    _Epilog
        ;
        ; Timer expired, try to remove from messge queue
actm00:
        ldd     Zl, Y+_p2l
        ldd     Zh, Y+_p2h      ; Recover pointer to queue
        rcall   _RemoveObject   ; p2 still point to TimerMessage object
        subi    tmp0, lo8(0)
        sbci    tmp1, hi8(0)    ; Test if in message queue
        brne    actm01
        std     Y+_r1l, Xl      ; If not found, stuff 0 into return registers
        std     Y+_r1h, Xh
actm01:
        rjmp    _Epilog
		
		_ENDFUNC AvrXCancelTimerMessage
