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

/*****************************************************************************/
#ifndef AVRXCHEADER
#define AVRXCHEADER
/*****************************************************************************/

#include <stdint.h>

#include <avr/pgmspace.h>

/*****************************************************************************/

#  define CTASK  __attribute__ ((noreturn))
#  define CTASKFUNC(A) void A(void) CTASK;\
    void A(void)

#  define BeginCritical() asm volatile ("cli\n")
#  define EndCritical()   asm volatile ("sei\n")

/*****************************************************************************/
/*****************************************************************************/
/***                                                                       ***/
/***                   K E R N E L   /   P R O C E S S E S                 ***/
/***                                                                       ***/
/*****************************************************************************/
/*****************************************************************************/

/*****************************************************************************
 *
 *  FUNCTION
 *      AvrXSetKernelStack
 *
 *  SYNOPSIS
 *      void *AvrXSetKernelStack(void *pNewStack)
 *
 *  DESCRIPTION
 *      Sets AvrX Stack to "pNewStack" or, if NULL then to the current stack
 *
 *  RETURNS
 *      Pointer to kernel stack
 *
 *****************************************************************************/
extern void *AvrXSetKernelStack(void *);

/*
    The process id is a chunk of eram that contains the state
    of a process.
*/
typedef struct ProcessID
{
    struct ProcessID  *next;
    uint8_t            flags;
#define AVRX_PID_Idle         (_BV(4))       /* Dead Task, don't schedule, resume or step */
#define AVRX_PID_Suspend      (_BV(5))       /* Mark task for suspension (may be blocked elsewhere) */
#define AVRX_PID_Suspended    (_BV(6))       /* Mark task suspended (it was removed from the run queue) */

    uint8_t            priority;
    void              *ContextPointer;
}
* pProcessID, ProcessID;

#define NOPID ((pProcessID)0)

struct AvrXKernelData
{
    struct ProcessID *RunQueue;
    struct ProcessID *Running;
    void             *AvrXStack;
    uint8_t           SysLevel;
};

/*****************************************************************************/
/*****************************************************************************/
/***                                                                       ***/
/***                         S E M A P H O R E S                           ***/
/***                                                                       ***/
/*****************************************************************************/
/*****************************************************************************/

/*
 Mutex semaphores are a simple linked list of waiting
 processes.  The mutex may have the following values:

 SEM_PEND         // Semaphore is reset waiting for a signal
 SEM_DONE         // Semaphore has been triggered.
 SEM_WAIT         // Something is waiting on the semaphore
                  // Any other value is the address of a processID
*/
#define AVRX_SEM_PEND ((Mutex)0)
#define AVRX_SEM_DONE ((Mutex)1)
#define AVRX_SEM_WAIT ((Mutex)2)

typedef pProcessID Mutex, *pMutex;     /* A mutex is a pointer to a process */

#define AVRX_MUTEX(A)\
        Mutex A

extern void AvrXSetSemaphore(pMutex);
extern void AvrXIntSetSemaphore(pMutex);
extern void AvrXWaitSemaphore(pMutex);

extern Mutex AvrXTestSemaphore(pMutex);
#define AvrXIntTestSemaphore(A) \
            AvrXTestSemaphore(A)

/*****************************************************************************
 *
 *  FUNCTION
 *      AvrXResetSemaphore
 *
 *  SYNOPSIS
 *      void AvrXResetSemaphore(pSystemObject)
 *
 *  DESCRIPTION
 *      Resets the semaphore.
 *
 *  RETURNS
 *      none
 *
 *****************************************************************************/
extern void AvrXResetSemaphore(pMutex);

/*****************************************************************************/
/*****************************************************************************/
/***                                                                       ***/
/***                    S Y S T E M    O B J E C T S                       ***/
/***                                                                       ***/
/*****************************************************************************/
/*****************************************************************************/

typedef struct SystemObject
{
    struct SystemObject *next;
    Mutex semaphore;    
}
* pSystemObject, SystemObject;


/*****************************************************************************
 *
 *  FUNCTION
 *      AvrXSetObjectSemaphore
 *      AvrXIntSetObjectSemaphore
 *
 *  SYNOPSIS
 *      void AvrXSetObjectSemaphore(pSystemObject)
 *      void AvrXIntSetObjectSemaphore(pSystemObject) 
 *
 *  DESCRIPTION
 *      Sets the semaphore within a System Object.
 *      The Int version is safe to be called from within an interrupt handler.
 *
 *  RETURNS
 *      none
 *
 *****************************************************************************/
extern void AvrXSetObjectSemaphore(pSystemObject);
extern void AvrXIntSetObjectSemaphore(pSystemObject);

/*****************************************************************************
 *
 *  FUNCTION
 *      AvrXResetObjectSemaphore
 *
 *  SYNOPSIS
 *      void AvrXResetObjectSemaphore(pSystemObject)
 *
 *  DESCRIPTION
 *      Resets the semaphore within a System Object.
 *
 *  RETURNS
 *      none
 *
 *****************************************************************************/
extern void AvrXResetObjectSemaphore(pSystemObject);

/*****************************************************************************
 *
 *  FUNCTION
 *      AvrXTestObjectSemaphore
 *
 *  SYNOPSIS
 *      void AvrXTestObjectSemaphore(pSystemObject)
 *
 *  DESCRIPTION
 *      Gets the state of the semaphore within a System Object.
 *
 *  RETURNS
 *      Semaphore state:
 *           SEM_PEND         // Semaphore is reset waiting for a signal
 *           SEM_DONE         // Semaphore has been triggered.
 *           SEM_WAIT         // Something is waiting on the semaphore
 *                            // Any other value is the address of a processID
 *
 *****************************************************************************/
extern Mutex AvrXTestObjectSemaphore(pSystemObject);

/*****************************************************************************
 *
 *  FUNCTION
 *      AvrXWaitObjectSemaphore
 *
 *  SYNOPSIS
 *      void AvrXWaitObjectSemaphore(pSystemObject)
 *
 *  DESCRIPTION
 *      Waits on the semaphore within a System Object.
 *
 *  RETURNS
 *      none
 *
 *****************************************************************************/
extern void AvrXWaitObjectSemaphore(pSystemObject);

/*****************************************************************************/
/*****************************************************************************/
/***                                                                       ***/
/***                           M E S S A G E S                             ***/
/***                                                                       ***/
/*****************************************************************************/
/*****************************************************************************/

/*
    Message Queues are really an extension of Semaphore
    queue.  They can be used for mutual exlusion (have a
    message be the baton) or as a general purpose message
    passing system.  This is needed for tasks that must block
    waiting for several things at once (timer, interrupt, etc)
*/
typedef struct MessageControlBlock
{
    SystemObject SObj;
}
* pMessageControlBlock, MessageControlBlock;

#define NOMESSAGE ((pMessageControlBlock)0)

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

extern pMessageControlBlock AvrXRecvMessage(pMessageQueue);

extern pMessageControlBlock AvrXWaitMessage(pMessageQueue);

extern void AvrXSendMessage(pMessageQueue, pMessageControlBlock);

extern void AvrXIntSendMessage(pMessageQueue, pMessageControlBlock);

#define AvrXAckMessage(A) \
        AvrXSetObjectSemaphore((pSystemObject)(A))
        
#define AvrXWaitMessageAck(A) \
        AvrXWaitObjectSemaphore((pSystemObject)(A))

#define AvrXTestMessageAck(A) \
        AvrXTestObjectSemaphore((pSystemObject)(A))

/*****************************************************************************/
/*****************************************************************************/
/***                                                                       ***/
/***                              T I M E R S                              ***/
/***                                                                       ***/
/*****************************************************************************/
/*****************************************************************************/

/*
    The timer queue manager is a service run in kernel mode and is tuned
    to minimize interrupt latency while queueing, tracking and dequeuing
    timers
*/
typedef struct TimerControlBlock
{
    struct SystemObject SObj;
    uint16_t count;
}
* pTimerControlBlock, TimerControlBlock;

#define NOTIMER ((pTimerControlBlock)0)

#define AVRX_TIMER(A) TimerControlBlock A

/*****************************************************************************
 *
 *  FUNCTION
 *      AvrXStartTimer
 *
 *  SYNOPSIS
 *      void AvrXStartTimer(pTimerControlBlock pTCB, uint16_t count)
 *
 *  DESCRIPTION
 *      Start a timer pTCB to run for count system ticks.
 *
 *  RETURNS
 *      none
 *
 *****************************************************************************/

extern void AvrXStartTimer(pTimerControlBlock, uint16_t);

/*****************************************************************************
 *
 *  FUNCTION
 *      AvrXCancelTimer
 *
 *  SYNOPSIS
 *      pTimerControlBlock AvrXCancelTimer(pTimerControlBlock pTCB)
 *
 *  DESCRIPTION
 *      Cancels timer pTCB immediately.  Any waiting task is scehduled to run.
 *
 *  RETURNS
 *      none
 *
 *****************************************************************************/

extern pTimerControlBlock AvrXCancelTimer(pTimerControlBlock);

/*****************************************************************************
 *
 *  FUNCTION
 *      AvrXDelay
 *
 *  SYNOPSIS
 *      void AvrXDelay(pTimerControlBlock pTCB, uint16_t count)
 *
 *  DESCRIPTION
 *      Utility function combining AvrXStartTimer() and AvrXWaitTimer().
 *
 *  RETURNS
 *      none
 *
 *****************************************************************************/

extern void AvrXDelay(pTimerControlBlock, uint16_t);

/*****************************************************************************
 *
 *  FUNCTION
 *      AvrXWaitTimer
 *
 *  SYNOPSIS
 *      void AvrXWaitTimer(pTimerControlBlock pTCB)
 *
 *  DESCRIPTION
 *      Blocking wait for a timer to complete.
 *
 *  RETURNS
 *      none
 *
 *****************************************************************************/

#define AvrXWaitTimer(A) \
        AvrXWaitObjectSemaphore((pSystemObject)(A))

/*****************************************************************************
 *
 *  FUNCTION
 *      AvrXTestTimer
 *
 *  SYNOPSIS
 *      void AvrXTestTimer(pTimerControlBlock pTCB)
 *
 *  DESCRIPTION
 *      Non-blocking check to see if a timer is running or not.
 *
 *  RETURNS
 *      Timer state:
 *           SEM_PEND         // Timer running but nothing waiting
 *           SEM_DONE         // Timer expired
 *           other            // Timer is running and something is waiting
 *
 *****************************************************************************/
        
#define AvrXTestTimer(A) \
        AvrXTestObjectSemaphore((pSystemObject)(A))

/*****************************************************************************
 *
 *  FUNCTION
 *      AvrXTimerHandler
 *
 *  SYNOPSIS
 *      void AvrXTimerHandler(void)
 *
 *  DESCRIPTION
 *      Kernel Function to be called by timer ISR.
 *      The simplest timer handler is:
 *
 *				AVRX_SIGINT(TIMER0_OVF_vect)
 *				{
 *					AvrXEnterKernel();          // Switch to kernel stack/context
 *					TCNT0 = TCNT0_INIT;
 *					AvrXTimerHandler();         // Call Time queue manager
 *					AvrXLeaveKernel();          // Return to tasks
 *				}
 *
 *  RETURNS
 *      none
 *
 *****************************************************************************/

extern void AvrXTimerHandler(void);

/*****************************************************************************/
/*****************************************************************************/
/***                                                                       ***/
/***                     T I M E R   M E S S A G E S                       ***/
/***                                                                       ***/
/*****************************************************************************/
/*****************************************************************************/
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

// Special versions of timer queue elements that get sent
// to a message queue when expired.

extern void AvrXStartTimerMessage(pTimerMessageBlock, uint16_t, pMessageQueue);
extern pMessageControlBlock AvrXCancelTimerMessage(pTimerMessageBlock, pMessageQueue);

/*****************************************************************************/
/*****************************************************************************/
/***                                                                       ***/
/***                             T A S K S                                 ***/
/***                                                                       ***/
/*****************************************************************************/
/*****************************************************************************/

/*
   The Task Control Block contains all the information needed
   to initialize and run a task.  It is stored in FLASH and is
   used only by AvrXInitTask()
*/
typedef struct
{
    void *r_stack;                  // Start of stack (top address-1)
    void (*start) (void);           // Entry point of code
    pProcessID pid;                 // Pointer to Process ID block
    uint8_t priority;           // Priority of task (0-255)
}
PROGMEM const TaskControlBlock;
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
#define AVRX_TASK(start, c_stack, priority) \
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

#define AVRX_EXTERNTASK(start)  \
  CTASKFUNC(start);             \
  extern TaskControlBlock start##Tcb; \
  extern ProcessID start##Pid

/*****************************************************************************
 *
 *  FUNCTION
 *      AvrXInitTask
 *
 *  SYNOPSIS
 *      void AvrXInitTask(TaskControlBlock *)
 *
 *  DESCRIPTION
 *      Initialises a task.
 *
 *  RETURNS
 *      none
 *
 *****************************************************************************/
extern pProcessID AvrXInitTask(TaskControlBlock *);

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
extern void AvrXRunTask(TaskControlBlock *);


extern void AvrXResume(pProcessID);
extern void AvrXSuspend(pProcessID);
extern void AvrXYield(void);
extern void AvrXIntReschedule(void);

/*****************************************************************************
 *
 *  FUNCTION
 *      AvrXTerminate
 *
 *  SYNOPSIS
 *      void AvrXTerminate(pProcessID)
 *
 *  DESCRIPTION
 *      Force any task to terminate.
 *
 *  RETURNS
 *      none
 *
 *****************************************************************************/
 extern void AvrXTerminate(pProcessID);

/*****************************************************************************
 *
 *  FUNCTION
 *      AvrXTaskExit
 *
 *  SYNOPSIS
 *      void AvrXTaskExit(void)
 *
 *  DESCRIPTION
 *      Called by a task to terminate itself.  From this point on the task can
 *      no longer be scheduled and remains in a zombie state.
 *
 *  RETURNS
 *      none
 *
 *****************************************************************************/
extern void AvrXTaskExit(void);

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
extern void AvrXHalt(void);

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
extern uint8_t AvrXPriority(pProcessID);

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
extern uint8_t AvrXChangePriority(pProcessID, uint8_t);

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
extern pProcessID AvrXSelf(void);

/*****************************************************************************
 *
 *  FUNCTION
 *      AvrXEnterKernel
 *      AvrXLeaveKernel
 *
 *  SYNOPSIS
 *      void AvrXEnterKernel(void)
 *      void AvrXLeaveKernel(void)
 *
 *  DESCRIPTION
 *      Switch to and from kernel context.  Must be first and last actions 
 *      inside interrupt handler.  E.g.,
 *
 *      AVRX_SIGINT(TIMEY_WIMEY_vect)
 *      {
 *          AvrXEnterKernel();
 *              .
 *              .
 *              .
 *          AvrXLeaveKernel();
 *      }
 *
 *  RETURNS
 *      None
 *
 *****************************************************************************/
extern void AvrXEnterKernel(void);
extern void AvrXLeaveKernel(void);

/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/
#endif /* AVRXCHEADER */
/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/
