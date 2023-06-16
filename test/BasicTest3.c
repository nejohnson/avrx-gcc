/*
 Basic Tasking Tests #3

 Exercises Timer Queue operations

 The following API covered:
    AvrXStartTimer
    AvrXCancelTimer
    AvrXTimerHandler
    AvrXDelay

 */

#include <avr/interrupt.h>

#include "avrx.h"
#include "hardware.h"

TimerControlBlock timer1, timer2, timer3, timer4, timer5;
Mutex TimerSemaphore;

/* This port corresponds to the "-W 0x20,-" command line option. */
#define special_output_port (*((volatile char *)0x20))

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
    AvrXIntSetSemaphore(&TimerSemaphore);  // Signal test task of tick
    AvrXTimerHandler();
    AvrXLeaveKernel();
}


AVRX_TASKDEF(task2, 60, 2)
{
    while(1)
    {
        AvrXStartTimer(&timer1, 5);
        AvrXDelay(&timer2, 3);
        AvrXCancelTimer(&timer1);
        if (AvrXTestTimer(&timer1) != AVRX_SEM_DONE)    // Canceling sets semaphore..
            {debug_puts("HALT@53");AvrXHalt();}
        debug_puts("2");
    }
}


AVRX_TASKDEF(task3, 60, 2)
{
    while(1)
    {
        AvrXDelay(&timer5, 3);
        debug_puts("3");
    }
}

AVRX_TASKDEF(task1, 60, 3)
{
    AvrXStartTimer(&timer1, 4); // Add timer to empty queue
    AvrXStartTimer(&timer2, 2); // Insert timer in front
    if ((timer2.count != 2) ||
        (timer1.count != 2))
        {debug_puts("HALT@73");AvrXHalt();}
    AvrXStartTimer(&timer3, 5); // Append timer on tail
    if ((timer3.count != 1) ||
        (timer2.count != 2) ||
        (timer1.count != 2))
        {debug_puts("HALT@78");AvrXHalt();}
    AvrXCancelTimer(&timer3);   // Cancel last timer
    if ((timer2.count != 2) ||
        (timer1.count != 2))
        {debug_puts("HALT@82");AvrXHalt();}
    AvrXStartTimer(&timer3, 5);
    AvrXCancelTimer(&timer2);   // Cancel first timer
    if ((timer3.count != 1) ||
        (timer1.count != 4))
        {debug_puts("HALT@87");AvrXHalt();}
    AvrXStartTimer(&timer2, 2);
    AvrXCancelTimer(&timer1);   // Cancel middle timer
    if ((timer3.count != 3) ||
        (timer2.count != 2))
        {debug_puts("HALT@92");AvrXHalt();}
    AvrXStartTimer(&timer1, 5); // Timer 1 & 3 are same values
    if ((timer1.count != 0) ||
        (timer3.count != 3) ||
        (timer2.count != 2))
        {debug_puts("HALT@97");AvrXHalt();}
    AvrXStartTimer(&timer4, 6); // Append another timer
    if ((timer4.count != 1) ||
        (timer1.count != 0) ||
        (timer3.count != 3) ||
        (timer2.count != 2))
        {debug_puts("HALT@103");AvrXHalt();}
        
    // Enable clock hardware and observe timers being processed
    TCNT0 = TCNT0_INIT;
	TCCR0 = TMC8_CK256;
	TIMSK = _BV(TOIE0);

    AvrXWaitSemaphore(&TimerSemaphore);
    
    if ((timer4.count != 1) ||
        (timer1.count != 0) ||
        (timer3.count != 3) ||
        (timer2.count != 1))   // Timer2 dec by one
        {debug_puts("HALT@116");AvrXHalt();}
    AvrXWaitTimer(&timer2);     // Wait two more ticks
    if ((timer4.count != 1) ||
        (timer1.count != 0) ||
        (timer3.count != 3))
        {debug_puts("HALT@121");AvrXHalt();}
    AvrXWaitTimer(&timer1);
    AvrXWaitTimer(&timer3);     // Timer 1 & 3 expire together

    if ((unsigned)timer1.SObj.next | (unsigned)timer3.SObj.next)  // Check both dequeued.
        {debug_puts("HALT@126");AvrXHalt();}
    if (timer4.count != 1)      // Timer 4 left to go.
        {debug_puts("HALT@128");AvrXHalt();}
        
    AvrXWaitTimer(&timer4);

    // Ok, static queue operations seem Ok.  Now try
    // hammering the queue while working on active timers.
    AvrXResume(&task2Pid);
    AvrXResume(&task3Pid);
    
    while(1)
    {
        AvrXStartTimer(&timer4, 3);
        AvrXCancelTimer(&timer4);
        debug_puts("1");
    }
}

int main(void)
{
    AvrXSetKernelStack(0);
    
    AvrXRunTask(&task1Tcb);
    AvrXInitTask(&task2Tcb);    // Init, but don't run until end of Task1
    AvrXInitTask(&task3Tcb);

    AvrXLeaveKernel();                   // Switch from AvrX Stack to first task
    while(1);
}
