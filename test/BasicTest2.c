/*
 Basic Tasking Tests #2

 Tests each semaphore call (not aliases)

 With Basic Test 1, this test pretty much covers basic tasking and semaphore
 operation.

 Runs through Set/Reset of Semaphores with things waiting and
 not waiting.  Tests actual state of semaphores to verify their action

 Test out other tasking stuff:
    AvrXTerminate
    AvrXResume      // Shouldn't resume a terminated task
    AvrXSuspend
    AvrXResetSemaphore

 NOTE: This test will generate a bunch of casting errors (warnings) when
 compiled.  No big deal...
 */

#include "avrx.h"
#include "hardware.h"

AVRX_MUTEX(Simple);
AVRX_TIMER(Object);
AVRX_MUTEX(Done);

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

AVRX_TASKDEF(task2, 20, 2)
{
    while(1)
    {
        AvrXWaitSemaphore(&Simple);
        debug_puts("2");
        AvrXSetSemaphore(&Done);
    }
}

AVRX_TASKDEF(task3, 20, 2)
{
    while(1)
    {
        AvrXWaitTimer(&Object);
        debug_puts("3");
        AvrXSetSemaphore(&Done);
    }
}

AVRX_TASKDEF(task1, 20, 3)
{
    while(1)
    {
        AvrXRunTask(&task2Tcb);       // Task blocks on Simple semaphore
        if (AvrXTestSemaphore(&Simple) != AVRX_SEM_WAIT)
            AvrXHalt();
        if (AvrXTestSemaphore(&Done) != AVRX_SEM_PEND)
            AvrXHalt();

        AvrXSuspend(&task2Pid);     // Still Queued, but marked for suspension
        AvrXSetSemaphore(&Simple);  // Ok, released, but now suspended
        if (AvrXTestSemaphore(&Done) != AVRX_SEM_PEND)
            AvrXHalt();
        if (AvrXTestSemaphore(&Simple) != AVRX_SEM_PEND)
            AvrXHalt();

        AvrXResume(&task2Pid);      // Ok, done and waiting on Simple again.
        if (AvrXTestSemaphore(&Done) != AVRX_SEM_DONE)
            AvrXHalt();
        if (AvrXTestSemaphore(&Done) != AVRX_SEM_PEND) // Test resets _DONE to _PEND
            AvrXHalt();
        if (AvrXTestSemaphore(&Simple) != AVRX_SEM_WAIT)
            AvrXHalt();

        AvrXSuspend(&task2Pid);     // Still Queued, but marked for suspension
        AvrXSetSemaphore(&Simple);  // Now _PEND
        AvrXSetSemaphore(&Simple);  // Now _DONE
        AvrXResetSemaphore(&Simple);// Now _PEND
        if (AvrXTestSemaphore(&Simple) != AVRX_SEM_PEND)
            AvrXHalt();
        AvrXResume(&task2Pid);      // Now blocked on Simple
        AvrXResetSemaphore(&Done);   // Should have been _DONE
        AvrXResetSemaphore(&Simple); // this should do nothing
        if (AvrXTestSemaphore(&Done) != AVRX_SEM_PEND)
            AvrXHalt();
        if (AvrXTestSemaphore(&Simple) != AVRX_SEM_WAIT)
            AvrXHalt();
        AvrXTerminate(&task2Pid);   // Mark task for termination
        AvrXSetSemaphore(&Simple);  // Release from Semaphore so it terminates.
        AvrXResume(&task2Pid);      // This should have no effect

        //
        // Now do it all again but with an Object...
        // use "Timer" since there is no data type "object"
        //

        AvrXRunTask(&task3Tcb);       // Task blocks on Simple semaphore
        if (AvrXTestTimer(&Object) != AVRX_SEM_WAIT)
            AvrXHalt();
        if (AvrXTestSemaphore(&Done) != AVRX_SEM_PEND)
            AvrXHalt();

        AvrXSuspend(&task3Pid);     // Still Queued, but marked for suspension
        AvrXSetSemaphore(&Object.SObj.semaphore);  // Ok, released, but now suspended
        if (AvrXTestSemaphore(&Done) != AVRX_SEM_PEND)
            AvrXHalt();
        if (AvrXTestTimer(&Object) != AVRX_SEM_PEND)
            AvrXHalt();

        AvrXResume(&task3Pid);      // Ok, done and waiting on Object again.
        if (AvrXTestSemaphore(&Done) != AVRX_SEM_DONE)
            AvrXHalt();
        if (AvrXTestSemaphore(&Done) != AVRX_SEM_PEND) // Test resets _DONE to _PEND
            AvrXHalt();
        if (AvrXTestTimer(&Object) != AVRX_SEM_WAIT)
            AvrXHalt();

        AvrXSuspend(&task3Pid);     // Still Queued, but marked for suspension
        AvrXSetSemaphore(&Object.SObj.semaphore);  // Now _PEND
        AvrXSetSemaphore(&Object.SObj.semaphore);  // Now _DONE
        AvrXResetSemaphore(&Object.SObj.semaphore);// Now _PEND
        if (AvrXTestTimer(&Object) != AVRX_SEM_PEND)
            AvrXHalt();
        AvrXResume(&task3Pid);      // Now blocked on Object
        AvrXResetSemaphore(&Done);   // Should have been _DONE
        AvrXResetSemaphore(&Object.SObj.semaphore); // this should do nothing
        if (AvrXTestSemaphore(&Done) != AVRX_SEM_PEND)
            AvrXHalt();
        if (AvrXTestTimer(&Object) != AVRX_SEM_WAIT)    // Should be waiting on semaphore
            AvrXHalt();
        AvrXTerminate(&task3Pid);
        AvrXSetSemaphore(&Object.SObj.semaphore);
        AvrXResume(&task3Pid);

		debug_puts("1");
    }
}



int main(void)
{
    AvrXSetKernelStack(0);

    AvrXRunTask(&task1Tcb);

    AvrXLeaveKernel();           // Switch from AvrX Stack to first task
    while(1);
}

