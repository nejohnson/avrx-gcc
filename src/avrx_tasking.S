/*
	avrx_tasking.S
	
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

        _MODULE avrx_tasking

;+
; --------------------------------------------------
; void AvrXEnterKernel(void)
;
; Pushes entire register context onto the stack, returning a frame pointer
; to the saved context.  If running in user mode upon entry (SysLevel == 0xFF)
; then switches the stack to the kernel and stores the frame pointer in the
; current processes PID.
;
; PASSED:       Nothing
; RETURN:       Y = Frame Pointer (See AvrX.inc)
;               R0 = pid.state (Single Step Flag) or 0
; ASSUMES:
; USES:         R14-R18, R26-R32, SysLevel
; CALLS:
; NOTES:        Does not muck with R16-R25 so passed parameters from C to
;               AvrX functions can be accessed directly after a call to Prolog.
;
;		Special case for IDLE: Do not save context or restore context when
;		transitioning from USER to Kernel context - it has no context when in
;		in user mode.  Do not put any code into the IDLE task - it will break!
;
;-
		_FUNCTION AvrXEnterKernel

AvrXEnterKernel:                      ; 3 cycles
		push	R29
		push	R28
		push	R27
		push	R26
		push	R25


		ldi		Yh, hi8(AvrXKernelData)
		ldi		Yl, lo8(AvrXKernelData)

		in		R25, _SFR_IO_ADDR(SREG)		; Save flags

		ldd		Xl, Y+SysLevel
		subi	Xl, lo8(-1)		; Carry set if results 0
		std		Y+SysLevel, Xl		; if already in kernel, then save context

		brcs	SaveContext

		ldd		Xl, Y+Running+NextL	; Check if current task = IDLE
		ldd		Xh, Y+Running+NextH
		or		Xh, Xl

		brne	SaveContext		; Carry cleared if results 0
		;
		; When interrupting IDLE, just reset the stack pointer to PRIOR the interrupt.
		; in Epilog, if still IDLE, control will return to the start of the IDLE loop.
		; I.E. we do not need the return address to IDLE, just the return address to the
		; caller of Prolog() (the interrupt handler).
		;
		; The code, below, is obscure, but cycle efficient.  The net effect is to reset
		; the stack to AvrXStack before returning to the interrupt handler.
		;
		in		Yl, _SFR_IO_ADDR(SPL)	;Idle task: do not bother saving registers, etc.
		in		Yh, _SFR_IO_ADDR(SPH)
		ldd		Zh, Y+6         	; Get return address
		ldd		Zl, Y+7
		adiw	Yl, 9			; Adjust pointer
		out		_SFR_IO_ADDR(SPL), Yl  	; This is cycle efficient, but obscure.
		out		_SFR_IO_ADDR(SPH), Yh
		ijmp				; ~37 cycles for IDLE task.

SaveContext:
		push	R24
        push    R23
        push    R22
        push    R21
        push    R20
        push    R19
        push    R18
        push    R17
        push    R16
        push    R15
        push    R14
        push    R13
        push    R12
        push    R11
        push    R10
        push    R9
        push    R8
        push    R7
        push    R6
        push    R5
        push    R4
        push    R3
        push    R2
        push    R1
        push    R0
        push    R25		; SREG
	
		in		Yl, _SFR_IO_ADDR(SPL)
		in		Yh, _SFR_IO_ADDR(SPH)         ; Grab frame pointer
		
		ldd		R25, Y+_R25	; Need to restore this register (used for SREG)
		ldd		tmp1, Y+_RetLo
		ldd		tmp2, Y+_RetHi
        std     Y+_R30, Zl
        std     Y+_R31, Zh      ; Swap return address with Z

		brcs	AlreadyInKernel ; Flags, still good, from above.
			
		ldi		Zh, hi8(AvrXKernelData)
		ldi		Zl, lo8(AvrXKernelData)
		ldd		Yh, Z+Running+NextH    ; Point to running task
		ldd		Yl, Z+Running+NextL
		in		Xh, _SFR_IO_ADDR(SPH)
		in		Xl, _SFR_IO_ADDR(SPL)
		std		Y+PidSP+NextH, Xh
		std		Y+PidSP+NextL, Xl

        ldd		tmp0, Z+AvrXStack+NextL
		out		_SFR_IO_ADDR(SPL), tmp0
		ldd		tmp0, Z+AvrXStack+NextH
		out		_SFR_IO_ADDR(SPH), tmp0       ; Swap to kernel stack
		mov		Yl, Xl
		mov		Yh, Xh		; restore frame pointer

AlreadyInKernel:                ; (85/102)
		clr     R1              ; R1 = __Zero__ for Avr-gcc
        mov     Zl, tmp1        ; 
        mov     Zh, tmp2
		ijmp			; Return to caller (89/106)
		
        _ENDFUNC AvrXEnterKernel

/*+
; --------------------------------------------------
; void AvrXLeaveKernel(void)
; _Epilog
;
; Restore previous context (kernel or user).
; If task has SingleStep flag set, then generate an interrupt
; before returning to the task.
;
; PASSED:
; RETURN:
; ASSUMES:      SysLevel >= 0 (running on kernel stack)
; USES:         SysLevel
; CALLS:
-*/
        _FUNCTION AvrXLeaveKernel
		
AvrXLeaveKernel:
        pop     R30
        pop     R30
		
		_PUBLIC _Epilog
_Epilog:
		ldi		Zl, lo8(AvrXKernelData)
		ldi		Zh, hi8(AvrXKernelData)
        BeginCritical
        ldd     R16, Z+SysLevel    ; Interrupts off..
        dec     R16
        std		Z+SysLevel, R16
        brge    SkipTaskSwap

        ldd     Yh, Z+RunQueue+NextH
        ldd     Yl, Z+RunQueue+NextL
        std     Z+Running+NextH, Yh
        std     Z+Running+NextL, Yl   ; Update current running task
        adiw    Yl, 0
        breq    _IdleTask

        ldd     Xh, Y+PidSP+NextH
        ldd     Xl, Y+PidSP+NextL
        out     _SFR_IO_ADDR(SPL), Xl
        out     _SFR_IO_ADDR(SPH), Xh         ; 20 cycles
SkipTaskSwap:                   ; 20/6
        pop     R0
        out     _SFR_IO_ADDR(SREG), R0
        pop     R0
        pop     R1
        pop     R2
        pop     R3
        pop     R4
        pop     R5
        pop     R6
        pop     R7
        pop     R8
        pop     R9
        pop     R10
        pop     R11
        pop     R12
        pop     R13
        pop     R14
        pop     R15
        pop     R16
        pop     R17
        pop     R18
        pop     R19
        pop     R20
        pop     R21
        pop     R22
        pop     R23
        pop     R24
        pop     R25
        pop     R26
        pop     R27
        pop     R28
        pop     R29
        pop     R30
        pop     R31
        EndCriticalReturn       ; 97/83 cycles with interrupts off

; Jump here if there are no entries in the _RunQueue.  Never return.  Epilog will
; take care of that.  NB - this code has *NO* context.  Do not put anything in
; here that uses registers as you will get hosed every time an interrupt occurs.

_IdleTask:
; Any interrupt will exit the Idle task
        sei   					; Enable interrupts
        sleep                   ; Power Down..
        rjmp    _IdleTask
		
        _ENDFUNC AvrXLeaveKernel

/*+
; --------------------------------------------------
; void AvrXResume(pProcessID)
;
; Takes a PID and inserts it into the run queue.
;
; Uses two flags, Suspended and Suspend to determine if a task is
; actually suspended, as opposed to just being marked for suspension.
; Mark _SUSP to make QueuePid not queue, Mark _SUSPENDED when attempt
; to queue is made (e.g. not blocked on a semaphore).  This is because
; we cannot "resume" something that is blocked on a semaphore, even though
; it is marked for suspension.
;
; PASSED:       R25:24 = PID to queue
; RETURNS:
; USES:
; ASSUMES:
; NOTES:
-*/
        _FUNCTION AvrXResume

AvrXResume:                     ; User Entry Point
        BeginCritical
        rcall   AvrXEnterKernel
        EndCritical


        mov     Zl, p1l
        mov     Zh, p1h

        ldd     tmp0, Z+PidState
        cbr     tmp0, BV(SuspendBit)
        std     Z+PidState, tmp0  ; clear Suspend flag

        sbrs    tmp0, SuspendedBit
        rjmp    ar00

        cbr     tmp0, BV(SuspendedBit)  ; If removed from run queue,
        std     Z+PidState, tmp0  ; Clear flag and queue it up.

        rcall   _QueuePid       ; If found, then add to run queue
ar00:
        rjmp    _Epilog
		
        _ENDFUNC AvrXResume
/*+
; --------------------------------------------------
; _QueuePid
;
; Takes a PID and inserts it into the run queue.  The run queue is sorted
; by priority.  Lower numbers go first.  If there are multiple tasks of equal
; priority, then the new task is appended to the list of equals (round robin)
;
; PASSED:       p1h:p1l = PID to queue
; RETURNS:      r1l:	-1 = suspended
;			0  = Top of run queue
;			1-N= Depth in run queue
; USES:         Z, tmp0-3 and SREG, RunQueue
; ASSUMES:
; NOTES:        Returns with interrupts on.
;		; 9/13/04 Preserves INTERRUPTS
-*/
        _FUNCTION _QueuePid

_QueuePid:                              ; Kernel entry point only
        mov     Zl, p1l
        mov     Zh, p1h
        ldi		tmp1, lo8(-1)

        ldd     tmp0, Z+PidState          ; Xh = Priority & Flags
        andi    tmp0, (BV(SuspendBit) | BV(IdleBit)) ; if marked Suspended or idle
        brne    _qpSUSPEND

		push	Yl		; 9/13/04
		push	Yh		; 9/13/04

        ldd     tmp2, Z+PidPriority
        ldi     Yl, lo8(AvrXKernelData+RunQueue)
        ldi     Yh, hi8(AvrXKernelData+RunQueue)
		in		tmp0, _SFR_IO_ADDR(SREG)
		cli
_qp00:
		inc		tmp1     		; Tmp1 = counter of insertion point.
    	mov     Zl, Yl                 	; 0 = head of run queue.
        mov     Zh, Yh
        ldd     Yl, Z+PidNext+NextL
        ldd     Yh, Z+PidNext+NextH     ; Z = current, X = Next
        adiw    Yl, 0
        breq    _qp01                   ; End of queue, continue
        ldd     tmp3, Y+PidPriority
        cp      tmp2, tmp3
        brsh    _qp00                   ; Loop until pri > PID to queue
_qp01:
        std     Z+NextH, p1h
        std     Z+NextL, p1l    ; Prev->Next = Object
        mov     Zh, p1h
        mov     Zl, p1l
        std     Z+NextH, Yh     ; Object->Next = Next
        std     Z+NextL, Yl
		pop		Yh		; 9/13/05
		pop		Yl		; 9/13/04
        mov		r1l, tmp1
		out		_SFR_IO_ADDR(SREG), tmp0
		ret			; 9/13/04

_qpSUSPEND:
		mov		r1l, tmp1
        sbr     tmp0, BV(SuspendedBit)  ; Mark suspended and return
        std     Z+PidState, tmp0
		ret			; 9/13/04

        _ENDFUNC _QueuePid

