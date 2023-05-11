/*
	avrx.inc

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

20020907 - Added _IOMACRO_H_ definition to prevent older GCC io.h files
	   from including C only header information

	 - Added #defines for _SFR_IO_ADDR() macro to make it a NOP for
	   pre GCC 3.2 compilers and for IAR (I wonder what they do for
	   extended I/O access?)

	   THIS works because avrx.inc loads "io.h" which loads "sfr-defs.h"
	   which defines _SFR_IO_ADDR() macro on the 3.2 compiler.
*/

#ifndef __AVRXINC
#define __AVRXINC
#include "avrx-ctoasm.inc"


// Define _IOMACROS_H_ to prevent GCC 3.02 io.h from including this
// C only file when using io.h for assembly code.  io.h called within
// avrx-io.h
#define _IOMACROS_H_
#include <avr/io.h> 

#  if !defined(_SFR_IO_ADDR)      /* GCC 3.2 compatibility */
#    define _SFR_IO_ADDR(A) (A)   /* earlier compilers do not use this */
#  endif
#  define _GLOBAL(Label, Size) .comm Label, Size
#  define _STATIC(Label, Size) .lcomm Label, Size
#  define _DATASECTION
#  define _CODESECTION
#  define _FUNCTION(A) \
        .global A $ \
        .func   A
#  define _ENDFUNC .endfunc
#  define _PUBLIC(A) .global A
#  define _EXTERN(A)
#  define _MODULE(A) .section .text.avrx.A
#  define _ENDMOD
#  define _END
#  define _DB .asciz
#  define MACRO .macro
#  define ENDM .endm
#  define _EQU(A,B) .equ A, B
#  define _ALIGN .balign 2


#ifndef BV
#define BV(A) (1<<A)
#endif

/*
 Frame Pointer Offsets for various registers after the return from
 a call to _Prolog

 Used mainly to restore registers used internally (rather than saving
 and restoring) and to stuff return values across _Epilog.

*/
#define _RetLo  33      /* Return address on stack big endian */
#define _R31    33      /* Registers saved in order (little endian) */
#define _RetHi  32
#define _R30    32
#define _R29    31
#define _R28    30
#define _R27    29
#define _R26    28
#define _R25    27
#define _R24    26
#define _R23    25
#define _R22    24
#define _R0     2
#define _SREG   1       /* SP points below last saved item */

#  define _p1l     26
#  define _p1h     27
#  define _p2l     24
#  define _p2h     25
#  define _r1l     26
#  define _r1h     27


/* ******** AVRX kernel data structure offsets
   These are offsets into a slab of memory called "AvrXKernelData"
   defined in avrx_tasking.s

   NB: CODE depends upon RunQueue being the FIRST item!
*/

#define RunQueue 0	/* Head of the run queue */
#define Running 2	/* Current running task */
#define AvrXStack 4	/* User defined stack location */
#define SysLevel 6	/* re-entry counter into kernel context */

#	define AvrXKernelDataSz 7

/* ******** TCB (Task Control Block) offsets */

#define TaskSP          0       /* Stack pointer */
#define TaskMain        2       /* Entry point of process */
#define TaskPid         4       /* Pointer to Process ID */
#define TaskFlags       6       /* Priority */

#define TaskSz  7

/* All objects have same link list structure
   Change this to convert to big-endian for direct C access
*/

#define NextL 0		/* Big-endian for C compatibility */
#define NextH 1

/******** PID (Process ID) block offsets */

#define PidNext         0       /* Next item on list (semaphore, run) */
#define PidState        2       /* Upper Nibble: Task flags, Lower Nibble :Priority */
#define PidPriority     3
#define PidSP           4       /* Context Pointer */
#define PidSz           6

/* ******* PID (Process ID) BLOCK BIT DEFINITIONS ******* */

/* PID.State  */

#define IdleBit         4       /* Dead Task, don't schedule, resume or step */
#define SuspendBit      5       /* Mark task for suspension (may be blocked elsewhere) */
#define SuspendedBit    6       /* Mark task suspended (it was removed from the run queue) */
#define SingleStep      7       /* Enable single step debug support */

/*+ --------------------------------------------------
SEMAPHORE BIT DEFINITIONS

 General Semaphore Flags.  When a task is waiting on the
 semaphore, the semaphore is set to the TASK ID.  Otherwise
 _PEND means the semaphore is set and _DONE means it has
 been triggered.  A task will only block if it tries to
 grab a _PEND semaphore.  Semaphores are initialized in the
 _PEND state and are left in the _PEND state after each
 complete operation.  Processes are allowed to queue on
 a semaphore.  This effectively make the semaphore a Mutex
 when it has been "set" prior to the first call.  When a process
 done with the mutex, it releases it by "setting" it again.
-*/
#define _PEND           0
#define _DONE           1

#define MtxSz           2

#define MUTEX(A) _GLOBAL(A, 2)

/*+ --------------------------------------------------

 Events

 Events replace PID's in semaphores.  The kernel recognizes
 semaphore values (contents) < 0x60 as events, indexes into
 a table and calls a routine.  Macros and segments are used
 to build the table.

 */
#define TIMERMESSAGE_EV 2
#define PERIODICTIMER_EV 3      /* Not implemented */
#define _LASTEV 0x5F

/* Timers */

#define TcbNext         0       /* Pointer in linked list */
#define TcbSemaphore    2       /* Associated semaphore */
#define TcbCount        4       /* Timer ticks till expired */
#define TcbQueue        6
#define TcbSz           6       /* Primitive Timer */
#define TmbSz           8       /* Timer Message */

#define TIMER(A) _GLOBAL(A, TcbSz)

#define TIMERMESSAGE(A) _GLOBAL(A, TmbSz)

/* Message Queue */

#define MsqSz           4       /* Head of message queue */

#define MsqMessage      0       /* Head of list of messages */
#define MsqPid          2       /* Head of list of waiting processes */

#define QcbSz           4       /* Queue Block Size (No data) */

#define QcbNext         0
#define QcbSemaphore    2       /* Return Receipt Semaphore */
#define QcbData         4       /* pointer to data/or data */

#define MESSAGEQ(A) _GLOBAL(A, MsqSz)

/* Fifos */

#define FifoSz 4        /* Size of AvrX Fifo Data Struct */

#define WriteSemaphore 0
#define ReadSemaphore 2
#define fBuffer 4

/* User Section */

#define f_buffersz 4    /* Size of buffer header */

#define f_Size 0
#define f_Count 1
#define f_In 2
#define f_Out 3
#define f_Data 4        /* Variable length */

#define FIFO(A, B, Size) \
        _GLOBAL(A, FifoSz) \
        _GLOBAL(B, (Size+f_Buffersz))

#ifndef AVRXCHEADER
/*+ -------------------------------------------------- 
 Handy Macros
*/
        MACRO  EndInterrupt
        reti
        ENDM

        MACRO BeginCritical
        cli
        ENDM

        MACRO  EndCritical
        sei
        ENDM

#define EndCriticalReturn EndInterrupt

/*
 Use this macro rather than a call to _Prolog, see
 version notes in AvrX.asm
*/
        MACRO  AVRX_Prolog
        BeginCritical
        rcall   IntProlog
        EndCritical
        ENDM
/*
 These register definitions are just handy aliases for the
 various index and word math registers
 */
#define		Wl 		R24
#define		Wh 		R25
#define		Xl 		R26
#define		Xh 		R27
#define		Yl 		R28
#define		Yh 		R29
#define		Zl 		R30
#define		Zh 		R31

#endif /*AVRXCHEADER */
#endif  /* __AVRXINC */
