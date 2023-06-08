/*
 	avrx_eeprom.c - Task-safe EEPROM access

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

static Mutex EEPromMutex;

/*****************************************************************************/
void AvrXEEPromInit(void)
{
	AvrXSetSemaphore(&EEPromMutex); 
}

/*****************************************************************************/
uint8_t AvrXReadEEProm(const uint8_t *p)
{
	uint8_t b;
	
	AvrXWaitSemaphore(&EEPromMutex);
	eeprom_busy_wait();
	b = eeprom_read_byte(p);
	AvrXSetSemaphore(&EEPromMutex);
	
	return b;
}

/*****************************************************************************/
uint16_t AvrXReadEEPromWord(const uint16_t *p)
{
	uint16_t w;
	
	AvrXWaitSemaphore(&EEPromMutex);
	eeprom_busy_wait();
	w = eeprom_read_word(p);
	AvrXSetSemaphore(&EEPromMutex);
	
	return w;
}

/*****************************************************************************/
void AvrXWriteEEProm(uint8_t *p, uint8_t b)
{
	AvrXWaitSemaphore(&EEPromMutex);
	eeprom_busy_wait();
	eeprom_write_byte(p, b);
	AvrXSetSemaphore(&EEPromMutex);
}

/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/
