/*
 Basic Tasking Tests #4

 Exercises Message API

 The following API covered:
    AvrXSendMessage
    AvrXRecvMessage
    AvrXWaitMessage
    AvrXAckMessage
    AvrXTestMessageAck
    AvrXWaitMessageAck

To run: Step through Task 1.  Set break points on each of the "AvrXAckMessage()"
calls and in the _Overflow0_ routine and run through making sure each is called.
The sequence is not important as long as all four get called regularly.

Alternatively run this on hardware and check the bits on PortB that are toggled
with each sucessfull run.
 */

#include <avr/interrupt.h>

#include "avrx.h"
#include "hardware.h"

/*
   One group of task def's will be dropped depending upon which
   compiler is being used.
*/

Mutex Sync2, Sync3;     // Synchronization semaphores between task1 and 2&3
MessageControlBlock Message1, Message2, Message3;
MessageQueue TestQueue;


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
    // This relies on us "priming the queue" in the main task

    if (AvrXTestMessageAck(&Message1) == AVRX_SEM_DONE)
        AvrXIntSendMessage(&TestQueue, &Message1);
    AvrXLeaveKernel();
}


AVRX_TASKDEF(task2, 20, 2)
{
    while(1)
    {
        AvrXSendMessage(&TestQueue, &Message2);
        AvrXWaitMessageAck(&Message2);
        debug_puts("2");
    }
}

AVRX_TASKDEF(task3, 20, 2)
{
    while(1)
    {
        AvrXSendMessage(&TestQueue, &Message3);
        AvrXWaitMessageAck(&Message3);
        debug_puts("3");
    }
}

AVRX_TASKDEF(task1, 20, 3)
{
    // Verify initial condition of empty

    if (AvrXRecvMessage(&TestQueue) != NOMESSAGE)
        AvrXHalt();

    // Send and receive a message

    AvrXSendMessage(&TestQueue, &Message1);
    if (AvrXRecvMessage(&TestQueue) != &Message1)
        AvrXHalt();

    // Check to see that queue is empty

    if (AvrXRecvMessage(&TestQueue) != NOMESSAGE)
        AvrXHalt();

    // Acknowledge the message

    AvrXAckMessage(&Message1);
    if (AvrXTestMessageAck(&Message1) != AVRX_SEM_DONE)
        AvrXHalt();

    // Queue up two messages

    AvrXSendMessage(&TestQueue, &Message1);
    AvrXSendMessage(&TestQueue, &Message2);
    if (AvrXWaitMessage(&TestQueue) != &Message1)
        AvrXHalt();
    AvrXAckMessage(&Message1);
    AvrXWaitMessageAck(&Message1);
    AvrXWaitMessage(&TestQueue);

    // Funky test to verify message queue is empty

    if ((unsigned)TestQueue.message || (unsigned)TestQueue.pid)
        AvrXHalt();

    // Ok, basic static queue mechanism checked out.  Now for
    // some dynamic stuff.
    // Task2 and Task3 jam messages onto the queue.  Task 1
    // runs low priority flushing the queue.

    AvrXRunTask(&task2Tcb);
    AvrXRunTask(&task3Tcb);

	TCNT0 = TCNT0_INIT;
	TCCR0 = TMC8_CK256;
	TIMSK = _BV(TOIE0);

    AvrXIntSendMessage(&TestQueue, &Message1); //Prime the interrupt queue

    while(1)
    {
        pMessageControlBlock msg;

        msg = AvrXWaitMessage(&TestQueue);
        AvrXAckMessage(msg);
        if ((msg = AvrXRecvMessage(&TestQueue)) != NOMESSAGE)
        {
            AvrXAckMessage(msg);
            debug_puts("1");
        }
    }
}


int main(void)
{
    AvrXSetKernelStack(0);

    AvrXRunTask(&task1Tcb);

    AvrXLeaveKernel();                   // Switch from AvrX Stack to first task
    while(1);
}



