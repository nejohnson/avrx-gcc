/*
 	avrx_taskinit.c - Task Initialization

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

#define PUSH_BYTE(b)	do{*pStack-- = (uint8_t)(b);} while(0)
							   
#define PUSH_WORD(w)	do{uint16_t ww = (uint16_t)(w); \
	    		           *pStack-- = ((ww)&0xFF);     \
	                       *pStack-- = (((ww)>>8)&0xFF);} while(0)

pProcessID AvrXInitTask(TaskControlBlock *pTCB)
{
	pProcessID pid;
	uint8_t *pStack;
	void(*pTask)(void);

	pStack   = (uint8_t *)    pgm_read_word(&pTCB->r_stack);
	pTask    = (void(*)(void))pgm_read_word(&pTCB->start);

	PUSH_WORD((uint16_t)pTask);

	//set R0-R31 and SREG to 0
	for (uint8_t i=0; i < 33; i++)
		PUSH_BYTE(0);

	pid = (pProcessID) pgm_read_word(&pTCB->pid);
	pid->ContextPointer = (void *)pStack;
	pid->priority       = pgm_read_byte(&pTCB->priority);
	pid->flags          = AVRX_PID_Suspend | AVRX_PID_Suspended;
	pid->next           = 0;

	return pid;
}

/*****************************************************************************/

void AvrXTaskExit(void)
{
	AvrXTerminate(AvrXSelf());
}

/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/
