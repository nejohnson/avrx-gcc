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
#include "avrxkernel.h"

/****
Notes:
0.   Z = R31:R30
     Y = R29:R28
     X = R27:R26

1. Once we're in kernel-space we can access the user-space args but for that 
we need a struct definition of a task's stack.  Something like this, bearing
in mind the stack pointer is post-decrement on push (pre-inc on pop), so it
grows DOWN:

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
	uint8_t  ToS;  // Top of stack
	uint8_t  Sreg; // System register

// Descriptive names to register pairs:
#define Z_REG	R.w[15]
#define Y_REG   R.w[14]
#define X_REG   R.w[13]

// GCC ABI puts function args in reg pairs starting at 25:24, then 23:22, then 21:20, i.e.
// *    RETVAL foo ( ARG0, ARG1, ARG2 )
// Note that single byte arguments are passed in two registers even though the
// top register is not used
//
#define ARG0_W    R.w[12]
#define ARG0_B    R.b[24] // R25 not used

#define ARG1_W    R.w[11]
#define ARG1_B    R.b[22] // R23 not used

#define ARG2_W    R.w[10]
#define ARG2_B    R.b[20] // R21 not used

// Return value in 25:24
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






/****************************************************************************/
/****************************************************************************/
/****************************************************************************/
/****************************************************************************/



#if 0




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


void AvrXTimerHandler(void)
{
	bool done = false;

	BeginCritical();
	_TimQLevel--;
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
	count--;
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



/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/




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
