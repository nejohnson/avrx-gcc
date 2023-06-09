# AvrX for GCC

## Introduction

AvrX-GCC is a Real Time Multitasking Kernel written for the Atmel AVR series of 
micro controllers and targetted to the GCC compiler.  It is derived from Larry
Barello's excellent AvrX project, which came with the following featureset:

* Fully pre-emptive, priority driven scheduler
* 255 priority levels. Tasks with the same priority round robin schedule 
  on a cooperative basis
* Semaphores can be used for either signaling/synchronization or as mutual 
  exclusion semaphores. Both blocking (wait) and non-blocking (test) calls are available
* Message Queues support passing information back and forth between Tasks. 
  Blocking and non-blocking calls are available for receipt and acknowledgement 
  of messages.
* Most non-blocking services are available when in interrupt routines.
* Time Queue Manager supports single event timers. Any process can set up a 
  timer and any process can wait on the expiration of a timer.
* Small: About 700-1000 words of code space needed for all features.  
* Fast: with a 10 MHz clock rate a 10 kHz system clock rate consumes about 20% 
  of the processor while actively tracking a timer (211 cycles including the
  interrupt and return).
* Many chores usually delegated to timer/counter subsystems can be written as 
  task level code with AvrX.

The Kernel is written in a mix of C and assembly.  Total kernel size varies from ~500 to 700 
words depending upon which version is being used.  Since the kernel is provided
as a library of routines, practical applications take up less space because not 
all functions are used.  AvrX-GCC supports 16-bit pointers and interfaces directly
with GCC compiler projects.  The number of tasks, semaphores, messages and timers is 
limited only by system SRAM.

RTOS's are nice in that the OS maintains state information for the programmer.
So, instead of a complicated state machine for each task, running off an interrupt
timer, the designer can write linear code (do this, wait for something, then do
that... etc).  In general the linear code is much easier to design, debug, 
understand and it is almost always smaller.

Since each task appears to have a complete CPU, it is also easier to develop 
independent modules that can be wired together later on.i

## Features

AvrX-GCC contains approximately 40 API in the following categories:

* Tasking
* Semaphores
* Timer Management
* Message Queues
* Task-safe EEPROM Read and Write

## Dependencies

AvrX-GCC is evolving!  Taking the original work of Larry and bringing it 
up-to-date and targetted exclusively at GCC.
To build and use AvrX-GCC you will need the following extra tools and libraries:

* AVR-GCC version 5.40 or later
* [AVR Libc](https://www.nongnu.org/avr-libc/) version 2.00 or later 

## Recommended Tools

* [Splint](https://splint.org/)
* [SimulAvr](https://www.nongnu.org/simulavr/)


# Programmer Interface

## Tasks

In order to support C, all registers need to be saved.
The minimum context is the 32 registers, SREG and the PC, for a total of 35 
bytes.  AvrXInitTask() fills all registers with 0x00.

Only the process context is saved on the Tasks stack. All other stack usage 
(Kernel and interrupts) are saved on the kernel stack. This minimizes the 
usage of SRAM at the expense of a context switch for the first interrupt or 
entry into a kernel API.  Subsequent interrupts (if nesting is allowed) simply
nest on the kernel stack.  A few API push one or two bytes onto the user stack.
But these API don't perform a context switch.

The remaining task information (Stack pointer, or, Context Pointer) are stored 
in the The Process ID block (PID).  The PID structure uses six bytes of SRAM.
The additional SRAM is the pointer for queuing processes and the status bits & 
Priority byte.

Task Control Block is a code (flash) based table that is used to initialize a 
process. See sample code or AvrX.inc or AvrX.h for layout information.

*	AvrXInitTask
*	AvrXRunTask
*	AvrXSuspend
*	AvrXResume
*	AvrXTaskExit
*	AvrXTerminate
*	AvrXHalt

## Semaphores

Semaphores are an SRAM pointer. They have three states: PEND, WAITING and DONE. 
When a process is blocked on a Semaphore, it is in the WAITING state. Multiple 
processes can queue waiting for a Semaphore. In the latter case the semaphore 
can be thought of as a Mutual Exclusion Semaphore. This is used in AvrX to 
control access to the EEPROM interface. More typically, Semaphores are used as 
communications flags between interrupt routines and tasks. They are also used to
synchronize tasks with Timers and Messages.

*	AvrXSetSemaphore
*	AvrXWaitSemaphore
*	AvrXTestSemaphore
*	AvrXResetSemaphore

There is also limited support for semaphores within interrupt context, only 
non-blocking features are available (for obvious reasons):

*	AvrXIntSetSemaphore
*	AvrXIntTestSemaphore

## Timers

Timer Control Blocks (TCB) are six bytes long. They manage a 16-bit count value. 
The timer queue manager maintains a sorted queue of timers, each adjusted such 
that the sum of all timers up to and including the timer in question equal the 
delay value specified for that timer. Canceling a timer re-adjusts the queue so 
all times come out correct.

API

*	AvrXStartTimer
*	AvrXTimerHandler
*	AvrXCancelTimer
*	AvrXWaitTimer
*	AvrXTestTimer
*	AvrXDelay

There is an additional variation of the timer queue block, the TimerMessageBlock. 
Timer messages are used in the TimerMessage example code.  In short, when the 
timer expires, a message is queued onto a message queue.  In this way a task can 
wait for multiple events by waiting on a message queue.

*	AvrXStartTimerMessage
*	AvrXCancelTimerMessage

## Message Queues

Message queues are defined with a Message Control Block (MCB) as the head of a 
queue. Messages can be queued by any process or interrupt handler and multiple 
processes can wait on a message queue (although there are not too many reasons 
for doing this). The MCB is four bytes long. The element is a link to the next 
message or process in the queue. The second element is the semaphore used to 
signal the process waiting on the queue. The message can be thought of as more 
flexible form of semaphore. They can be queued up and acknowledged, or simply 
treated as a baton for a mutual exclusion semaphore.  Data or a pointer can be
appended to the end of a message, etc.

*	AvrXSendMessage
*	AvrXRecvMessage
*	AvrXWaitMessage
*	AvrXAckMessage
*	AvrXTestMessageAck
*	AvrXWaitMessageAck

Inside interrupt handlers it is also possible to send messages:

*	AvrXIntSendMessage

## SystemObjects

AvrX is built around the notion of a system object. System Objects contain a 
link and a semaphore followed by zero or more bytes of data. Process Objects 
(PID) can queue on the Run Queue or on a Semaphore. Timer Control Blocks (TCB) 
can only be queued on the Timer Queue. Message Control Blocks (MCB) may queued 
only on a Message Queue. Processes wait on objects (timers, messages, message 
ack) by queuing (waiting) on the embedded semaphore in the object.

The only restriction would be for future AVR chips that directly support more 
than 64kb of SRAM: all AvrX structures would have to be located in the first 
64kb of ram.

The main limit to system size is the available SRAM for process stacks, which 
need a minimum of 10 to 35 bytes (depending upon version) to store a process 
context. There is no additional stack needed for processing interrupts as long 
as they use the AvrX semantics. Stacks can anywhere in the first 64k of SRAM 
space. For best performance at least the kernel stack should be in on-chip-SRAM.

*	AvrXSetObjectSemaphore
*	AvrXResetObjectSemaphore
*	AvrXWaitObjectSemaphore
*	AvrXTestObjectSemaphore

And within interrupts:

*	AvrXIntSetObjectSemaphore
*	AvrXIntTestObjectSemaphore

## System Stack

AvrX requires a stack large enough to handle all possible nested interrupts.
Each entry into the kernel pushes 10 to 35 bytes onto the stack (the standard 
context and a return address), interrupt handlers may push more. AvrX API may 
temporarily push a couple more bytes. AvrXStack is defined to be the top of 
SRAM and it is the designers job to make sure there is sufficient room between 
the AvrXStack and the upper limit of SRAM data.

AvrX does not control or limit the nesting level. However, if your design 
requires more stack space than one context per interrupt source then the design
might be broken or the processor simply too slow for the amount of work being 
done.

Note, by default the AvrX Time Queue Manager keeps the interrupts enabled and 
can potentially re-enter itself. The timer manager will unwind keeping track of 
the number of ticks. Although overall, no system ticks will be lost, the 
precision of time delay events will suffer (jitter).  Also, Kernel stack will 
grow an additional 10 to 35 bytes with each nesting. On a 10 MHz part a 10 kHz 
tick rate should be plenty of time, even with significant other interrupts 
(50% load), to prevent any re-entry and stack growth.

*    void AvrXSetKernelStack(void *pStack)

If you pass a NULL stack pointer, then AvrX will take the current SPL/SPH and 
make that the kernel stack.  This API only makes sense as the first executable 
line in your applications "main()" code.  See the samples for details.

## Macros

Macros are supplied to simplify the task of declaring AvrX data structures and 
placing them in the correct segments.

    AVRX_TASK(Start, StackSz, Priority)
	AVRX_TASKDEF(Start, StackSz, Priority)
	AVRX_EXTERNTASK(Start)
	
	AVRX_SIGINT(vector)

    AVRX_TIMER(timer)

    AVRX_MUTEX(mutex)
	
	AVRX_MESSAGE(msg)
	
	AVRX_MESSAGEQ(msgq)

## Detailed API descriptions

Please refer to the source.  Each function as pretty complete descriptions in 
the header.  Sample code illustrates how to declare and call routines.

Some thoughts on structuring code

Not all interrupts need to be passed through AvrX. In fact, one of my favorite 
techniques is to have Timer0 interrupt at a great speed, say 150k/sec perform 
some action (PWM and Encoder reading) and then once every 150 cycles then 
call \_IntProlog, then AvrXTimerHandler, then \_Epilog. The call to \_IntProlog 
will enable interrupts and the high speed code will continue to run. Make sure 
you reset your counter before dropping into the AvrX code!


