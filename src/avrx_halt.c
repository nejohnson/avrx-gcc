/*
 	avrx_halt.c

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
void AvrXHalt(void)
{
	asm ("cli\r");
	while(1);
}
