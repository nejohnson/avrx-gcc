#ifndef AVRXCHEADER
#define AVRXCHEADER

#include <stdint.h>
#include <avr/pgmspace.h>

/* Kernel *************************************************************/

extern void * AvrXSetKernelStack(void *);

/* Tasks **************************************************************/

typedef struct _avrxPCB
{
    struct _avrxPCB   *next;
    uint8_t            flags;
#define AVRX_PID_Idle         (_BV(4))
#define AVRX_PID_Suspend      (_BV(5))
#define AVRX_PID_Suspended    (_BV(6))

    uint8_t            priority;
    void              *pStack;
} avrxPCB, *avrxPID;
#define AVRX_NOPID ((avrxPID)0)

/* Return address, 32 registers, Status Register */
#define _AVRX_MINSTK(35)

/* Decorate task functions with compiler attributes */
#define _AVRX_TASKFUNC(A)                   \
   void A(void) __attribute__ ((noreturn)); \
   void A(void)

#define AVRX_STKNAME(A) A ## Stk
#define AVRX_PIDNAME(A) A ## Pid

/* Define a task */
#define AVRX_TASKDEF(TASKNAME, STACKSZ)                  \
   uint8_t AVRX_STKNAME(TASKNAME)[STACKSZ+_AVRX_MINSTK]; \
   avrxPID AVRX_PIDNAME(TASKNAME);                       \
   _AVRX_TASKFUNC(TASKNAME)

/* Interrupt handler */
#define AVRX_SIGINT(vector) \
   ISR(vector, ISR_NAKED)

/* External definition of a task */
#define AVRX_EXTERNTASK(TASKNAME)  \
   _AVRX_TASKFUNC(TASKNAME);       \
   extern AVRX_PIDNAME(TASKNAME);

extern avrxPID AvrXTaskInit(uint8_t *, void (*)(void), avrxPID, uint8_t);
extern void AvrXRunTask(uint8_t *, void (*)(void), avrxPID, uint8_t);

extern avrxPID AvrXSelf(void);
extern uint8_t AvrXGetPriority(avrxPID);
extern uint8_t AvrXSetPriority(avrxPID, uint8_t);
extern void AvrXResume(avrxPID);
extern void AvrXSuspend(avrxPID);
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
 extern void AvrXTerminate(avrxPID);

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

typedef avrxPID Mutex, *pMutex;     /* A mutex is a pointer to a process */

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
    avrxPID pid;        /* List of processes */
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
 *	AVRX_SIGINT(TIMER0_OVF_vect)
 *	{
 *		AvrXEnterKernel();          // Switch to kernel stack/context
 *		TCNT0 = TCNT0_INIT;
 *		AvrXTimerHandler();         // Call Time queue manager
 *		AvrXLeaveKernel();          // Return to tasks
 *	}
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



#endif /* AVRXCHEADER */
