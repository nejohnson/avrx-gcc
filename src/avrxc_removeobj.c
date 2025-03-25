/*
   avrxc_removeobj.c - Searches a queue for an object to remove

   Copyright (c)2024    Neil Johnson (neil@njohnson.co.uk)

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

/*****************************************************************************
 *
 *  FUNCTION
 *      _avrxRemoveObject
 *
 *  SYNOPSIS
 *      pProcessID _avrxRemoveObject(pProcessID pQueue, pProcessID pObject)
 *
 *  DESCRIPTION
 *      Walks the Queue looking for the Object, then removes it.
 *
 *      Before:
 *
 *          pQueue              pObject          pNext
 *       +---------+          +---------+     +---------+
 *       |   next  +--/.../-->|   next  +---->|   next  +----->NOPID
 *       +---------+          +---------+     +---------+
 *       |   ...   |          |   ...   |     |   ...   |
 *       +---------+          +---------+     +---------+
 *
 *      After:
 *
 *          pQueue               pNext (returned)
 *       +---------+          +---------+
 *       |   next  +--/.../-->|   next  +----->NOPID
 *       +---------+          +---------+
 *       |   ...   |          |   ...   |
 *       +---------+          +---------+
 
 *         pObject
 *       +---------+
 *       |   next  +----->NOPID
 *       +---------+
 *       |   ...   |
 *       +---------+
 *
 *
 *  RETURNS
 *      Pointer to next object in the Queue, else NOPID if not found or there
 *      is no next object.
 *
 *****************************************************************************/

pProcessID _avrxRemoveObject(pProcessID pQueue, pProcessID pObject)
{
    while (pQueue->next)
    {
        if (pQueue->next == pObject)
	{
	    _avrxRemoveNextObject(pQueue);
            return pQueue->next;
	}
        pQueue = pQueue->next;
    }
    return NOPID;    
}

/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/
