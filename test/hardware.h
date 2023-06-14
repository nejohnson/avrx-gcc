/*
    hardware.h

    Define my hardware dependent stuff here
    
    31-May-2002 - Added check for mega128
*/
#define CPUCLK 			( 8000000L )

/* Timers and counters */

#define TICKRATE 		( 1000 )

/* Peripheral initialization */

#define TCNT0_INIT 		(0xFF - CPUCLK/256/TICKRATE)

#define TMC8_CK256 		( _BV( CS02 ) )


