/*
    avrxeeprom.h - AvrX Utility - EEPROM

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
#ifndef AVRXEEPROM_H
#define AVRXEEPROM_H
/*****************************************************************************/

/*****************************************************************************
 *
 *  FUNCTION
 *      AvrXEEPromInit
 *
 *  SYNOPSIS
 *      void AvrXEEPromInit(void)
 *
 *  DESCRIPTION
 *      Sets up the AvrX EEPROM driver.
 *      AvrX provides some EEPROM access routines that control access to the 
 *      hardware via a semaphore.  This semaphore needs to be "set" prior to 
 *      using the access routines.
 *
 *  RETURNS
 *      none
 *
 *****************************************************************************/
extern void AvrXEEPromInit(void);

/*****************************************************************************
 *
 *  FUNCTION
 *      AvrXReadEEProm
 *
 *  SYNOPSIS
 *      uint8_t AvrXReadEEProm(const uint8_t *p)
 *
 *  DESCRIPTION
 *      Reads a single byte from EEPROM address 'p'
 *
 *  RETURNS
 *      The read byte
 *
 *****************************************************************************/
extern uint8_t AvrXReadEEProm(const uint8_t *);

/*****************************************************************************
 *
 *  FUNCTION
 *      AvrXReadEEPromWord
 *
 *  SYNOPSIS
 *      uint16_t AvrXReadEEPromWord(const uint16_t *p)
 *
 *  DESCRIPTION
 *      Reads a single word from EEPROM address 'p'
 *
 *  RETURNS
 *      The read word
 *
 *****************************************************************************/
extern uint16_t AvrXReadEEPromWord(const uint16_t *);

/*****************************************************************************
 *
 *  FUNCTION
 *      AvrXWriteEEProm
 *
 *  SYNOPSIS
 *      void AvrXWriteEEProm(uint8_t *p, uint8_t b)
 *
 *  DESCRIPTION
 *      Writes a single byte 'b' to EEPROM address 'p'
 *
 *  RETURNS
 *      none
 *
 *****************************************************************************/
extern void AvrXWriteEEProm(uint8_t *, uint8_t);

/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/
#endif /* AVRXEEPROM_H */
/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/
