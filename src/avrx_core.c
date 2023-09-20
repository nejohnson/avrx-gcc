/*
    avrx_core.c - Kernel Core experiments

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

#include "avrx.h"
#include "avrxcore.h"

/****
Notes:
0.   Z = R31:R30
     Y = R29:R28
     X = R27:R26

1. Once we're in kernel-space we can access the user-space args but for that 
we need a struct definition of a task's stack.  Something like this, bearing
in mind the stack pointer is pre-decrement on push, so it grows DOWN.

      /_______________/
      |               | STACKTOP
      |               |
      |---------------|
      |    RetLo      | [SP+35]
      |    RetHi      | [SP+34]
      |    R31        | [SP+33]
      |    R30        | [SP+32]
      |    ....       |
      |    R2         | [SP+4]
      |    R1         | [SP+3]
      |    R0         | [SP+2]
      |    SREG       | [SP+1]
      |---------------|
      |               |  <-- SP
      /               /


struct TaskStackFrame {
	uint8_t  ToS;
	uint8_t  Sreg;
// Add some descriptive names to register pairs
#define Z_REG	R.w[15]
#define Y_REG   R.w[14]
#define X_REG   R.w[13]
// GCC ABI puts args in reg pairs starting at 25:24, then 23:22, then 21:20
#define P1      R.w[12]
#define P2      R.w[11]
#define P3      R.w[10]
// and return value in 25:24
#define RETVAL  R.w[12]
	union {
		uint8_t  b[32]; // 8-bit access
		uint16_t w[16]; // 16-bit access
	} R;
	uint16_t RetAddr;
};




2. Move to have each function in its own source file where is makes sense.  This
gives a clean separation of functionality and makes changes easier to track
between parts of the API.


****/

/*****************************************************************************/

int8_t _avrxQueuePid(pProcessID pPid)
{
    int8_t counter = -1;

    if (pPid->flags & (AVRX_PID_Idle | AVRX_PID_Suspend))
    {
        pPid->flags |= AVRX_PID_Suspended;
    }
    else
    {
        pProcessID pCurr, pNext = (pProcessID)(&(AvrXKernelData.RunQueue));
        uint8_t sreg = SREG;
        cli();

        do {
            counter++;
            pCurr = pNext;
            pNext = pCurr->next;
        } while ((pNext != NOPID) && pNext->priority > pPid->priority);

        pCurr->next = pPid;
        pPid->next  = pNext;

        SREG = sreg;
    }

    return counter;
}

/*****************************************************************************/

void _avrxAppendObject(pProcessID pQueue, pProcessID pObject)
{
    while(pQueue->next)
        pQueue = pQueue->next;
    
    pQueue->next  = pObject;
    pObject->next = NOPID;
}

/*****************************************************************************/

/* Note: tell GCC not to inline this function to avoid code bloat */
pProcessID __attribute__ ((noinline)) _avrxRemoveObjectAt(pProcessID pPrev, pProcessID pObject)
{
    pPrev->next = pObject->next;
    pObject->next = NOPID;
    return pPrev->next;    
}

/*****************************************************************************/

pProcessID _avrxRemoveFirstObject(pProcessID pQueue)
{
    return pQueue->next
        ? _avrxRemoveObjectAt(pQueue, pQueue->next) 
        : NOPID;
}

/*****************************************************************************/

pProcessID _avrxRemoveObject(pProcessID pQueue, pProcessID pObject)
{
    while (pQueue->next)
    {
        if (pQueue->next == pObject)
            return _avrxRemoveObjectAt(pQueue, pObject);
        pQueue = pQueue->next;
    }
    return NOPID;    
}

/*****************************************************************************/

void AvrXDelay(pTimerControlBlock pTCB, uint16_t count)
{
	AvrXStartTimer(pTCB, count);
	AvrXWaitTimer(pTCB);
}

/*****************************************************************************/

extern void _Epilog(void);

#if 0

void AvrXStartTimer(pTimerControlBlock pTCB, uint16_t count)
{
	if(count==0)
		AvrXSetObjectSemaphore(&pTCB->SObj);
	else
    {
	BeginCritical();
        AvrXEnterKernel();
        EndCritical();

        pTimerControlBlock *Y, *Z = &_TimerQueue;

        BeginCritical();
        _TimQLevel--;
        EndCritical();

    ast00:
        Y = Z;
        Z = Y->next;
        if (Z==NOPID) goto ast01;

        uint16_t currcount = (uint16_t)(*Z->TcbCount->Next);

        if (currcount < count) goto ast00;

        (uint16_t)(*Z->TcbCount->Next) = currcount - count;

    ast01:
        *Y->next = pTcb;
        Y = pTcb;
        Y->next = Z;
        (uint16_t)(*Y->TcbCount->Next) = count;
        TimerHandler();
        _Epilog();
    }
}




void AvrXTimerHandler(void)
{
	bool done = false;

	BeginCritical();
	_TimQLevel–-;
	if(_TimQLevel==0xFF) done = true;
	EndCritical();
	if(!done)
		_avrxWalkTimerQueue();
}


void _avrxWalkTimerQueue(void)
{
	pTimerControlBlock Y = _TimerQueue->next;
	If (Y==NULL)
	{
		TimerHandler();
		Return;
	}

	uint16_t count = (uint16_t)(*Y->TcbCount->Next);
	count–-;
	(uint16_t)(*Y->TcbCount->Next) = count;

	while((uint16_t)(*Y->TcbCount->Next)==0)
	{
		pTimerControlBlock X = Y->next;
		_TimerQueue->next = X;
		Y->next = Z;

		pMutex mtx = Y->TcbSemaphore->next;
		if (mtx != TIMERMESSAGE_EV)
		{
			AvrXIntSendMessage(Y->TcbQueue->next, Y);
		}
		else
		{
			Mtx = Y;
			AvrXIntSetObjectSemaphore(mtx);
		}

		if(X == NULL)
			break;

		Y = X;
		Z = Y->TcbCount->next;
	}

	TimerHandler();
}
void _avrxTimerHandler(void)
{
	bool done = false;
	BeginCritical();
	_TimQLevel++;
	done = !_TimQLevel;
	EndCritical();

	if(!done) _avrxWalkTimerQueue();
}



pMessage AvrXRecvMessage(pMessageQueue pMQ)
{
	pMessageQueue Z = pMQ;
	pMessage pM = NULL;

	BeginCritical();
	pM = (pMessage)_RemoveFirstObject(Z);
	AvrXResetSemaphore(Z->sem);
	EndCritical();

	return pM;
}


void AvrXTerminate(pProcessID pPID)
{
	BeginCritical();
        AvrXEnterKernel();
        EndCritical();

	pPID->flags = AVRX_PID_Idle;
	BeginCritical();
	_RemoveObject(AvrXKernelData.RunQueue, pPID);
	_Epilog();
}



void AvrXResume(pProcessID pPID)
{
	BeginCritical();
        AvrXEnterKernel();
        EndCritical();

	/* Clear the suspend flag */
	pPID->flags &= ~AVRX_PID_Suspend;

	/* If removed from the run queue, then remove the suspended
 	* bit and put it on the queue.
 	*/
	if(pPID->flags & AVRX_PID_Suspended)
	{
		pPID->flags &= ~AVRX_PID_Suspended;
		_avrxQueuePid(pPID);
	}
	_Epilog();
}



void AvrXSendMessage(pMessageQueue pMQ, pMessageControlBlock pMCB)
{
	BeginCritical();
        AvrXEnterKernel();
        EndCritical();

	AvrXIntSendMessage(pMQ, pMCB);
	_Epilog();
}




void AvrXIntSendMessage(pMessageQueue pMQ, pMessageControlBlock pMCB)
{
	uint8_t sreg = SREG;
	cli();
	_avrxAppendObject(pMQ, pMCB);
	AvrXIntSetObjectSemaphore(pMCB);
}


void AvrXStartTimerMessage(pTimerMessageBlock pTMB, 
                           uint16_t count, 
                           pMessageQueue pMQ)
{
    if(count==0)
        AvrXSendMessage(pMQ, pTMB);
    else
    {
        pTMB->TcbQueue->next = pMQ;
        pTMB->TcbSemaphore->next = TIMERMESSAGE_EV;
        AvrXStartTimer(...sort out args...);
    }
}

void AvrXSuspend(pProcessID pPID)
{
	BeginCritical();
        AvrXEnterKernel();
        EndCritical();

	Z = pPID;
	Z->flags |= AVRX_PID_Suspend;

	BeginCritical();
	if(_RemoveObject(AvrXKernelData.RunQueue, pPID))
		Z->flags |= AVRX_PID_Suspended;

	_Epilog();
}







void AvrXTerminate(pProcessID pPID)
{
	BeginCritical();
        AvrXEnterKernel();
        EndCritical();

	pPID->flags |= AVRX_PID_Idle;

	BeginCritical();
	_RemoveObject(AvrXKernelData.RunQueue, pPID);
	_Epilog();
}





void AvrXYield(void)
{
	BeginCritical();
        AvrXEnterKernel();
        EndCritical();

	BeginCritical();
	pProcessID pSelf = AvrXKernelData.RunQueue->next;
	_RemoveObject(AvrXKernelData.RunQueue, pSelf);
	EndCritical();
	_QueuePid(pSelf);
	_Epilog();
}






pMessageControlBlock AvrXRecvMessage(pMessageQueue pMQ)
{
	pMessageControlBlock pMCB;
	Z = pMQ;
	BeginCritical();
	pMCB = _RemoveFirstObject(pMQ);
	AvrXResetSemaphore(pMCB->sem);
	EndCritical();
	return pMCB;
}




pMessageControlBlock AvrXWaitMessage(pMessageQueue pMQ)
{
	Z = pMQ;
	pMessageControlBlock pMCB;
		
	do {
        BeginCritical();
        pMCB = _RemoveFirstObject(Z);
        EndCritical();

        if(!pMCB)
            break;

        AvrXWaitObjectSemaphore(something here);
    } while(1);

    AvrXResetObjectSemaphore(something here);
    return pMCB;
}




void AvrXSetSemaphore(pMutex pSem)
{
	uint8_t qd = AvrXIntSetSemaphore(pSem);
	
	if(!qd && AvrXKernelData.SysLevel==0)
		return;

	_Epilog();
}



uint8_t AvrXIntSetSemaphore(pMutex pSem)
{
	Z = pSem;

	uint8_t sreg = SREG;
	cli();

	if(Z->next < AVRX_SEM_DONE)
	{
		Z->next = AVRX_SEM_DONE;
		SREG = sreg;
		return 1;
	}

	if(Z->next <= LASTEV)
	{
		SREG = sreg;
		return -1;
	}

	pProcessID pPID = _RemoveObjectAt(...);
	SREG = sreg;
	return !!_QueuePid(pPID);
}


void AvrXIntReschedule(void)
{
	Z = AvrXKernelData->RunQueue->next;
	if(!Z)
	return;

RunQueue->next = Z->next;
_QueuePid(Z->next);
}



void AvrXWaitSemaphore(pMutex pSem)
{
	Z = pSem;

	BeginCritical();

	if(Z->next == DONE)
	{
		Z->next = PEND;
		EndCriticalReturn();
	}

	AvrXEnterKernel();

	Z = AvrXKernelData.RunQueue;
	p2 = Z->Running->next;
	pPid = _RemoveObject(...);
	_AppendObject(pPid);

	_Epilog();
}



pTimerControlBlock AvrXCancelTimer(pTimerControlBlock pTCB)
{
	BeginCritical();
        FP *fp = AvrXEnterKernel();
        EndCritical();

	AvrXIntSetObjectSemaphore(pTCB);

	pTimerControlBlock pObj = (pTimerControlBlock *)(&fp->R[24]);

	BeginCritical;

	if (pNext = _avrxRemoveObject( _TimerQueue, pObj ) )
	{
		pNext->TcbCount += pObj->TcbCount;

		*(pTimerControlBlock *)(&fp->R[24]) = pObj;
	}
	else
	{
		*(pTimerControlBlock *)(&fp->R[24]) = NULL;
	}
	_Epilog();
}


pMessageControlBlock AvrXCancelTimerMessage(pTimerMessageBlock pTMB, pMessageQueue pMQ)
{
	BeginCritical();
        FP *fp = AvrXEnterKernel();
        EndCritical();

	BeginCritical();
	pNext = _RemoveObject(_TimerQueue, pTMB);
	if (pNext)
	{
		Y = pTMB;
		p2 = Y->TcbCount;
		r1 = Z->TcbCount;
		r1 += p2;
		Z->TcbCount = r1;
	}
	else
	{
		Z = *(pMessageQueue *)(&fp->R[22]);
		pNext = _RemoveObject(Z, pTMB);
		if (!pNext)
			*(pTimerControlBlock *)(&fp->R[24]) = NULL;
	}

	_Epilog();
}




#endif

/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/
