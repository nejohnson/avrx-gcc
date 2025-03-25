#ifndef AVRXKERNEL_H
#define AVRXKERNEL_H

#include <stdint.h>
#include <avr/interrupt.h>
#include <avr/io.h>

#include "avrx.h"

struct _avrxKernelData
{
    avrxPID           RunQueue;
    avrxPID           Running;
    void             *AvrXStack;
    /* SysLevel:
     *  0xFF  == User Mode
     *  else  == Kernel Mode
     */
    uint8_t           SysLevel;    
};
#define AVRX_SYSLEVEL_USER	( 0xFF )

extern struct _avrxKernelData _avrxKernelData;

/*****************************************************************************/
/**                            MACROS                                       **/
/*****************************************************************************/

#define _avrxBeginCritical() 		asm volatile ("cli\n")
#define _avrxEndCritical()   		asm volatile ("sei\n")

#define _avrxCriticalEnter()       	uint8_t _avrxSREG = SREG; _avrxBeginCritical()
#define _avrxCriticalLeave()       	SREG = _avrxSREG

#define _avrxCriticalScopeEnter()       { _avrxCriticalEnter()
#define _avrxCriticalScopeLeave()       _avrxCriticalLeave() }

/*****************************************************************************/
/**                         GLOBAL DATA                                     **/
/*****************************************************************************/

extern struct AvrXKernelData   _avrxAvrXKernelData;
extern pTimerControlBlock      _avrxTimerQueue;
extern uint8_t                 _avrxTimerQLevel;

/*****************************************************************************/
/**                         CORE FUNCTIONS                                  **/
/*****************************************************************************/

extern int8_t 			_avrxQueuePid(avrxPID);
extern void 			_avrxAppendObject(avrxPID, avrxPID);
extern avrxPID 		_avrxRemoveObject(avrxPID, avrxPID);
extern avrxPID 		_avrxRemoveNextObject(avrxPID);

/*****************************************************************************
 *
 *  FUNCTION
 *      _Epilog
 *
 *  SYNOPSIS
 *      void _Epilog(void)
 *
 *  DESCRIPTION
 *      Low level kernel function that restores previous context.
 *      In C functions it MUST be the last line of code so that it is jumped
 *      to rather than called.
 *
 *  RETURNS
 *      none
 *
 *****************************************************************************/

extern void _Epilog(void);

/*****************************************************************************
 *
 *  FUNCTION
 *      AVRXENTERKERNEL
 *
 *  SYNOPSIS
 *      void AVRXENTERKERNEL(pUserFrame)
 *
 *  DESCRIPTION
 *      Macros that makes a special call to the AvrXEnterKernel function
 *      which returns a pointer to the User Context in the Z register.
 *
 *      For example:
 *
 *              cli();
 *              uint8_t *pUserContext;
 *              AvrXEnterKernel(pUserContext);
 *              sei();
 *
 *      If the pUserContext is not used then this generates a single call
 *      instruction without saving registers.
 *
 *  RETURNS
 *      Address of the current User Context.
 *
 *****************************************************************************/

#define AVRXENTERKERNEL(pUserFrame) \
	__asm__ __volatile__ ("rcall AvrXEnterKernel" : "=z" (pUserFrame): : "cc" );

/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/
#endif /* AVRXKERNEL_H */
/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/
