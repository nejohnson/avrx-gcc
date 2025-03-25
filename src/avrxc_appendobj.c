/*
   avrxc_appendobj.c - Append an object on a queue

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
 *      _avrxAppendObject
 *
 *  SYNOPSIS
 *      void _avrxAppendObject(pProcessID pQueue, pProcessID pObject)
 *
 *  DESCRIPTION
 *      Adds the pObject onto the end of pQueue.
 *
 *  RETURNS
 *      Nothing
 *
 *****************************************************************************/

void _avrxAppendObject(pProcessID pQueue, pProcessID pObject)
{
    /* Walk the queue until we get to the end. */
    while(pQueue->next)
        pQueue = pQueue->next;
    
    /* Then append as the new last item */
    pQueue->next  = pObject;
    pObject->next = NOPID;
}

/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/
