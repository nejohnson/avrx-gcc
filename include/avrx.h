/*
	avrx.h - AvrX Public Interface Definitions

	Copyright (c)1998 - 2002 Larry Barello (larry@barello.net)
	Copyright (c)2023        Neil Johnson (neil@njohnson.co.uk)

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
#ifndef AVRXCHEADER
#define AVRXCHEADER

#include <stdint.h>


#  define FLASH  __attribute__ ((progmem))
#  define EEPROM __attribute__ ((section(".eeprom")))
#  define NAKED  __attribute__ ((naked))
#  define CTASK  __attribute__ ((noreturn))
#  define NAKEDFUNC(A) void A(void) NAKED;\
    void A(void)
#  define CTASKFUNC(A) void A(void) CTASK;\
	void A(void)
#  define INTERFACE

#  define BeginCritical() asm volatile ("cli\n")
#  define EndCritical()   asm volatile ("sei\n")

/*
    void * AvrXSetKernelStack(char *newstack)

    Sets AvrX Stack to "newstack" or, if NULL then to the current stack
*/
void *AvrXSetKernelStack(void *bval);
/*
    The process id is a chunk of eram that contains the state
    of a process.
*/
typedef struct ProcessID
{
    struct ProcessID  *next;
    uint8_t flags, priority;
    void *ContextPointer;
}
* pProcessID, ProcessID;

struct AvrXKernelData
{
    struct ProcessID *RunQueue;
    struct ProcessID *Running;
    void             *AvrXStack;
    uint8_t           SysLevel;
};


#define NOMESSAGE ((pMessageControlBlock)0)
#define NOTIMER ((pTimerControlBlock)0)
#define NOPID ((pProcessID)0)
#define SEM_PEND ((Mutex)0)
#define SEM_DONE ((Mutex)1)
#define SEM_WAIT ((Mutex)2)     // AvrXTestSemaphore(): Something waiting

/*
 Mutex semaphores are a simple linked list of waiting
 processes.  The mutex may have the following values:

 SEM_PEND         // Semaphore is reset waiting for a signal
 SEM_DONE         // Semaphore has been triggered.
                  // Any other value is the address of a processID
*/
typedef pProcessID Mutex, *pMutex;     /* A mutex is basically a pointer to a process */

#define AVRX_MUTEX(A)\
        Mutex A

INTERFACE void AvrXSetSemaphore(pMutex);
INTERFACE void AvrXIntSetSemaphore(pMutex);
INTERFACE void AvrXWaitSemaphore(pMutex);
INTERFACE Mutex AvrXTestSemaphore(pMutex);
INTERFACE Mutex AvrXIntTestSemaphore(pMutex);
INTERFACE void AvrXResetSemaphore(pMutex);
INTERFACE void AvrXResetObjectSemaphore(pMutex);
/*
    Message Queues are really an extension of Semaphore
    queue.  They can be used for mutual exlusion (have a
    message be the baton) or as a general purpose message
    passing system.  This is needed for tasks that must block
    waiting for several things at once (timer, interrupt, etc)
*/
typedef struct MessageControlBlock
{
    struct MessageControlBlock *next;
    Mutex semaphore;
}
* pMessageControlBlock, MessageControlBlock;

#define AVRX_MESSAGE(A) \
        MessageControlBlock A

typedef struct MessageQueue
{
    pMessageControlBlock message;    /* List of messages */
    pProcessID pid;        /* List of processes */
}
* pMessageQueue, MessageQueue;

#define AVRX_MESSAGEQ(A)\
        MessageQueue A

INTERFACE pMessageControlBlock AvrXRecvMessage(pMessageQueue);
INTERFACE pMessageControlBlock AvrXWaitMessage(pMessageQueue);
INTERFACE void AvrXSendMessage(pMessageQueue, pMessageControlBlock);
INTERFACE void AvrXIntSendMessage(pMessageQueue, pMessageControlBlock);
INTERFACE void AvrXAckMessage(pMessageControlBlock);
INTERFACE void AvrXWaitMessageAck(pMessageControlBlock);
INTERFACE Mutex AvrXTestMessageAck(pMessageControlBlock);
/*
    The timer queue manager is a service run in kernel mode and is tuned
    to minimize interrupt latency while queueing, tracking and dequeuing
    timers
*/
typedef struct TimerControlBlock
{
    struct TimerControlBlock *next;
    Mutex semaphore;
    uint16_t count;
}
* pTimerControlBlock, TimerControlBlock;
/*
    A special version of timers that send messages rather than firing
    a semaphore.
*/
typedef struct TimerMessageBlock
{
    union
    {
        struct MessageControlBlock mcb;
        struct TimerControlBlock tcb;
    } u;
    struct MessageQueue *queue;
}
* pTimerMessageBlock, TimerMessageBlock;


#define AVRX_TIMER(A) TimerControlBlock A

INTERFACE void AvrXStartTimer(pTimerControlBlock, uint16_t);
INTERFACE pTimerControlBlock AvrXCancelTimer(pTimerControlBlock);
INTERFACE void AvrXDelay(pTimerControlBlock, uint16_t);
INTERFACE void AvrXWaitTimer(pTimerControlBlock);
INTERFACE Mutex AvrXTestTimer(pTimerControlBlock);

INTERFACE void AvrXTimerHandler(void);    // Kernel Function to be called by timer ISR

// Special versions of timer queue elements that get sent
// to a message queue when expired.

INTERFACE void AvrXStartTimerMessage(pTimerMessageBlock, uint16_t, pMessageQueue);
INTERFACE pMessageControlBlock AvrXCancelTimerMessage(pTimerMessageBlock, pMessageQueue);

/*
   The Task Control Block contains all the information needed
   to initialize and run a task.  It is stored in FLASH and is
   used only by AvrXInitTask()
*/
typedef struct
{
    void *r_stack;          		// Start of stack (top address-1)
    void (*start) (void);   		// Entry point of code
    pProcessID pid;         		// Pointer to Process ID block
    uint8_t priority;       	// Priority of task (0-255)
}
FLASH const TaskControlBlock;
/*
    A series of macros to ease the declaration of tasks
    and access to the resulting data structures.

AVRX_TASK(start, stacksz, priority)
	Declare task data structures and forward reference to task
AVRX_TASKDEF(start, stacksz, priority)
	Declare task data structure and the top level C
	declaration (AVRX_TASK + C function declaration)
AVRX_SIGINT(vector)
	Declare the top level C declaration for an
	interrupt handler
AVRX_EXTERNTASK(start)
	Declare external task data structures
PID(start)
	Return the pointer to the task PID
TCB(start)
	Return the pointer to the task TCB
*/

#define MINCONTEXT 35           // 32 registers, return address and SREG
#define AVRX_TASK(start, c_stack, priority)	\
    uint8_t start ## Stk [c_stack + MINCONTEXT] ; \
    CTASKFUNC(start); \
    ProcessID start ## Pid; \
    TaskControlBlock start ## Tcb = \
    { \
        &start##Stk[sizeof(start##Stk)-1] , \
        start, \
        &start##Pid, \
        priority \
    }

#define AVRX_TASKDEF(start, c_stack, priority) \
    AVRX_TASK(start, c_stack, priority); \
    CTASKFUNC(start)

#define AVRX_SIGINT(vector)\
  ISR(vector, ISR_NAKED)

#define PID(start) &start##Pid
#define TCB(start) (&start##Tcb)

#define AVRX_EXTERNTASK(start)	\
  CTASKFUNC(start);				\
  extern TaskControlBlock start##Tcb; \
  extern ProcessID start##Pid

/*****************************************************************************
 *
 *  FUNCTION
 *      AvrXRunTask
 *
 *  SYNOPSIS
 *      void AvrXRunTask(TaskControlBlock *)
 *
 *  DESCRIPTION
 *      Initialises a task and then schedules it for running.
 *
 *  RETURNS
 *      none
 *
 *****************************************************************************/
INTERFACE void AvrXRunTask(TaskControlBlock *);

INTERFACE pProcessID AvrXInitTask(TaskControlBlock *);

INTERFACE void AvrXResume(pProcessID);
INTERFACE void AvrXSuspend(pProcessID);
INTERFACE void AvrXBreakPoint(pProcessID);
INTERFACE uint8_t AvrXSingleStep(pProcessID);
INTERFACE uint8_t AvrXSingleStepNext(pProcessID);

INTERFACE void AvrXTerminate(pProcessID);
INTERFACE void AvrXTaskExit(void);

/*****************************************************************************
 *
 *  FUNCTION
 *      AvrXHalt
 *
 *  SYNOPSIS
 *      void AvrXHalt(void)
 *
 *  DESCRIPTION
 *      Halt the system, wait for reset
 *
 *  RETURNS
 *      Never returns, it's the very last thing you ever do....
 *
 *****************************************************************************/
INTERFACE void AvrXHalt(void);

INTERFACE void AvrXWaitTask(pProcessID);
INTERFACE Mutex AvrXTestPid(pProcessID);


/*****************************************************************************
 *
 *  FUNCTION
 *      AvrXPriority
 *
 *  SYNOPSIS
 *      uint8_t AvrXPriority(pProcessID p)
 *
 *  DESCRIPTION
 *      Get a process's current priority.
 *
 *  RETURNS
 *      The process's current priority.
 *
 *****************************************************************************/
INTERFACE uint8_t AvrXPriority(pProcessID);

/*****************************************************************************
 *
 *  FUNCTION
 *      AvrXChangePriority
 *
 *  SYNOPSIS
 *      uint8_t AvrXChangePriority(pProcessID p, uint8_t priority)
 *
 *  DESCRIPTION
 *      Changes the priority of process 'p' to 'priority'.
 *
 *  RETURNS
 *      The previous priority
 *
 *****************************************************************************/
INTERFACE uint8_t AvrXChangePriority(pProcessID, uint8_t);

/*****************************************************************************
 *
 *  FUNCTION
 *      AvrXSelf
 *
 *  SYNOPSIS
 *      pProcessID AvrXSelf(void)
 *
 *  DESCRIPTION
 *      Gets the current process's pointer to its ProcessID
 *
 *  RETURNS
 *      Pointer to this process's ProcessID.
 *      Assumes the current process is top of the run queue.
 *
 *****************************************************************************/
INTERFACE pProcessID AvrXSelf(void);


INTERFACE void IntProlog(void);
INTERFACE void Epilog(void);

/*****************************************************************************/
/*****************************************************************************/

/*
 * AvrX provides some EEPROM access routines that control access to the hardware
 * via a semaphore.  This semaphore needs to be "set" prior to using the access
 * routines.
 */
 
/*****************************************************************************
 *
 *  FUNCTION
 *      AvrXEEPromInit
 *
 *  SYNOPSIS
 *      void AvrXEEPromInit(void)
 *
 *  DESCRIPTION
 *      Sets up the AvrX EEPROM system.
 *
 *  RETURNS
 *      none
 *
 *****************************************************************************/
INTERFACE void AvrXEEPromInit(void);

/*****************************************************************************
 *
 *  FUNCTION
 *      AvrXReadEEProm
 *
 *  SYNOPSIS
 *      uint8_t AvrXReadEEProm(const uint8_t *p)
 *
 *  DESCRIPTION
 *      Reads a single byte from EEPROM address 'p'
 *
 *  RETURNS
 *      The read byte
 *
 *****************************************************************************/
INTERFACE uint8_t AvrXReadEEProm(const uint8_t *);

/*****************************************************************************
 *
 *  FUNCTION
 *      AvrXReadEEPromWord
 *
 *  SYNOPSIS
 *      uint16_t AvrXReadEEPromWord(const uint16_t *p)
 *
 *  DESCRIPTION
 *      Reads a single word from EEPROM address 'p'
 *
 *  RETURNS
 *      The read word
 *
 *****************************************************************************/
INTERFACE uint16_t AvrXReadEEPromWord(const uint16_t *);

/*****************************************************************************
 *
 *  FUNCTION
 *      AvrXWriteEEProm
 *
 *  SYNOPSIS
 *      void AvrXWriteEEProm(uint8_t *p, uint8_t b)
 *
 *  DESCRIPTION
 *      Writes a single byte 'b' to EEPROM address 'p'
 *
 *  RETURNS
 *      none
 *
 *****************************************************************************/
INTERFACE void AvrXWriteEEProm(uint8_t *, uint8_t);

/*****************************************************************************/
#endif /* AVRXCHEADER */
/*****************************************************************************/
