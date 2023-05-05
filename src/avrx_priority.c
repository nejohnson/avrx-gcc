/*
 	avrx_priority.c

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

#include "avrx.h"

extern struct AvrXKernelData AvrXKernelData;

/*****************************************************************************
 *
 *  FUNCTION
 *      AvrXChangePriority
 *
 *  SYNOPSIS
 *      unsigned char AvrXChangePriority(pProcessID p, unsigned char priority)
 *
 *  DESCRIPTION
 *      Changes the priority of process 'p' to 'priority'.
 *
 *  RETURNS
 *      The old priority
 *
 *****************************************************************************/
unsigned char AvrXChangePriority(pProcessID p, unsigned char priority)
{
	unsigned char t = p->priority;
	p->priority = priority;
	return t;
}

/*****************************************************************************
 *
 *  FUNCTION
 *      AvrXSelf
 *
 *  SYNOPSIS
 *      pProcessID AvrXSelf(void)
 *
 *  DESCRIPTION
 *      Get the current process ID.
 *
 *  RETURNS
 *      Pointer to this process's ProcessID.
 *      Assumes the current process is top of the run queue.
 *
 *****************************************************************************/
pProcessID AvrXSelf(void)
{
	return AvrXKernelData.RunQueue;
}

/*****************************************************************************
 *
 *  FUNCTION
 *      AvrXPriority
 *
 *  SYNOPSIS
 *      unsigned char AvrXPriority(pProcessID p)
 *
 *  DESCRIPTION
 *      Get a process's priority.
 *
 *  RETURNS
 *      The process's priority.
 *
 *****************************************************************************/
unsigned char AvrXPriority(pProcessID p)
{
	return p->priority;
}
