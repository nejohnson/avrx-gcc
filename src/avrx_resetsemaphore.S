/*
	avrx_resetsemaphore.S

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

        _MODULE(avrx_resetsemaphore)
        _CODESECTION

/*+
; -----------------------------------------------
; pMutex AvrXResetSemaphore(pMutex)
; pMutex AvrXResetObjectSemaphore(pMutex)
;
; Force a semaphore into the _PEND state.  This is almost identical
; to SetSemaphore, but the end state is always _PEND rather than,
; possibly _DONE
;
; Usable in either USER or KERNEL code.
;
; PASSED:       R25:R24 = Semaphore
; RETURNS:      
; USES:         Z, X, flags
; STACK:
; SIDE EFFECTS: 
; Notes:        It does not make sense to reset a semaphore that has
;               a process waiting, so just skip that situation.
-*/
        _FUNCTION(AvrXResetObjectSemaphore)
        
AvrXResetObjectSemaphore:
        mov     Zl, p1l
        mov     Zh, p1h
        adiw    Zl, 2
        rjmp    ars00
        _ENDFUNC
        
        _FUNCTION(AvrXResetSemaphore)
        
AvrXResetSemaphore:
        mov     Zh, p1h
        mov     Zl, p1l
ars00:
        BeginCritical
        ldd     tmp2, Z+NextL
        ldd     tmp3, Z+NextH
        subi    tmp2, lo8(_DONE)
        sbci    tmp3, hi8(_DONE)
        breq    ars01           ; If it was _DONE, now it is _PEND
                                ; otherwize restore Pointer
        subi    tmp2, lo8(-_DONE)
        sbci    tmp3, hi8(-_DONE)
ars01:
        std     Z+NextH, tmp3 
        std     Z+NextL, tmp2
        EndCriticalReturn
        _ENDFUNC
        _END

