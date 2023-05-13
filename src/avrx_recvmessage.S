/*
	avrx_recvmessage.S

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

		_MODULE avrx_recvmessage.S

/*+
; --------------------------------------------------
; pMessage AvrXRecvMessage(pMessageQueue)
;
; Removes the first item in a queue. Return Zero if empty.
;
; PASSED:       R25, R24 = Queue head
; RETURNS:      R25, R24 = Object = 0 if list empty
; USES:         Z, X, Flags, R23:R22
; CALLS:
; ASSUMES:      Null terminated list
; NOTES:        If queue empty, then reset queue semaphore
;               
;
-*/
		_FUNCTION AvrXRecvMessage

AvrXRecvMessage:
        mov     Zl, p1l
        mov     Zh, p1h
        BeginCritical
        rcall   _RemoveFirstObject
        subi    p1l, lo8(-2)
        sbci    p1h, hi8(-2)
        rcall   AvrXResetSemaphore      ; Note, interrupt enabled here
        mov     r1l, p2l
        mov     r1h, p2h
        EndCriticalReturn

		_ENDFUNC AvrXRecvMessage

