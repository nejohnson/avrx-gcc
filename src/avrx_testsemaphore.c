/*
 	avrx_testsemaphore.c - Test Semaphore, INT safe

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

#include <avr/interrupt.h>
#include <avr/io.h>

#include <avrx.h>

/*****************************************************************************/
Mutex AvrXTestSemaphore(pMutex mtx)
{
	Mutex retval;
	
	/* Enter critical region, saving the SREG on the way in and turning off
	   interrupts. 
	*/
	uint8_t sreg = SREG;
	cli();
	
	retval = *mtx;
	
	if (*mtx == AVRX_SEM_DONE)
	{
		*mtx = AVRX_SEM_PEND;
	}
	else if (*mtx != AVRX_SEM_PEND)
	{
		retval = AVRX_SEM_WAIT;
	}

	/* Restore the SREG.  If we were in USER space then this will reset the 
	   I flag (previously disabled) to re-enable interrupts.  If in KERNEL
	   space then the I flag was already disabled and will remain disabled.
	*/
	SREG = sreg;
	return retval;
}
		
/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/
