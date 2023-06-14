/*
 Basic Tasking Tests #1

 Exercises basic semaphore constructs, Interrupt handling and tasking

 The following API covered:
        AvrXRunTask()
            AvrXResume()        //Indirectly covered
        Epilog()
        IntProlog()
        AvrXIntTestSemaphore()
            AvrXTestSemaphore() //Indirectly covered
        AvrXIntSetSemaphore()
            AvrXSetSemaphore()  //Indirectly covered
        AvrXWaitSemaphore()
        AvrXChangePriority()
        AvrXSelf()

 The code queues tasks on the run queue and the Semaphore:
        in the front
        inserted in the middle
        appended to the end

 The timer code flushes the tasks off the semaphore to start
 the process over again.

 To run this test simply examine the state of the LED bits
 (PORTB) and make sure each task is running regularly.

 20050918 - Added "hardware.h" to project which covers major hardware
 			variations for timer setup, etc.

 20010524 - Make sure the AvrXTestSemaphore() is returning SEM_WAIT
 when multiple tasks are blocked.

 */
 
#include <avr/interrupt.h>

#include "avrx.h"
#include "hardware.h"

AVRX_MUTEX(TimerSemaphore);

/* This port corresponds to the "-W 0x20,-" command line option. */
#define special_output_port (*((volatile char *)0x20))

/* This port corresponds to the "-R 0x22,-" command line option. */
#define special_input_port  (*((volatile char *)0x22))

/* Poll the specified string out the debug port. */
void debug_puts(const char *str) {
  const char *c;

  for(c = str; *c; c++)
    special_output_port = *c;
}


AVRX_SIGINT(TIMER0_OVF_vect)
{
    AvrXEnterKernel();
    TCNT0 = TCNT0_INIT;
    while(AvrXIntTestSemaphore(&TimerSemaphore) == AVRX_SEM_WAIT)
        AvrXIntSetSemaphore(&TimerSemaphore);   // Flush all waiting tasks
    AvrXIntSetSemaphore(&TimerSemaphore);       // Set Semaphore (short path)
    AvrXLeaveKernel();
}

AVRX_TASKDEF(task1, 20, 2)
{
    while(1)
    {
        AvrXWaitSemaphore(&TimerSemaphore);
        debug_puts("1");
    }
}

// Task two ping pongs between 1 and 3 priority
AVRX_TASKDEF(task2, 20, 1)
{
    unsigned char tPri = 3;
    while(1)
    {
        AvrXWaitSemaphore(&TimerSemaphore);
        tPri = AvrXChangePriority(AvrXSelf(), tPri);
        debug_puts("2");
    }
}

// Task three ping pongs between 4 and 0 priority

AVRX_TASKDEF(task3, 20, 4)
{
    unsigned char tPri = 0;
    while(1)
    {
        AvrXWaitSemaphore(&TimerSemaphore);
        tPri = AvrXChangePriority(AvrXSelf(), tPri);
        debug_puts("3");
    }
}


int main(void)
{
    AvrXSetKernelStack(0);

	MCUCR = _BV(SE);      // Initialize Timer Hardware
	TCNT0 = TCNT0_INIT;
	TCCR0 = TMC8_CK256;
	TIMSK = _BV(TOIE0);    // Enable Timer overflow interrupt

    AvrXRunTask(&task1Tcb);
    AvrXRunTask(&task2Tcb);
    AvrXRunTask(&task3Tcb);

    AvrXLeaveKernel();                   // Switch from AvrX Stack to first task
    while(1);
}
