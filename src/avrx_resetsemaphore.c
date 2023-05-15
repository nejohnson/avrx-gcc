/*
 	avrx_resetsemaphore.c - Reset a semaphore

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

/*****************************************************************************/
/**
	Notes
	
	Force a semaphore into the _PEND state.  This is almost identical
	to SetSemaphore, but the end state is always _PEND rather than,
	possibly _DONE.

	Usable in USER code only.
	Does NOT preserve state of I flag, is always set on exit.

	It does not make sense to reset a semaphore that has
    a process waiting, so just skip that situation.
	
	Sem State		Transition
	----------		------------
	SEM_PEND		SEM_PEND  	(already reset, waiting)
	SEM_DONE		SEM_PEND	(action)
	else			no change	(active wait)

**/
void AvrXResetSemaphore(pMutex mtx)
{
	BeginCritical();
	
	if ( *mtx == AVRX_SEM_DONE )
		*mtx = AVRX_SEM_PEND;
	
	EndCritical();	
}

/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/
