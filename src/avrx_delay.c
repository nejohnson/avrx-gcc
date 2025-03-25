#include "avrx.h"

void AvrXDelay(pTimerControlBlock pTCB, uint16_t count)
{
   AvrXStartTimer(pTCB, count);
   AvrXWaitTimer(pTCB);
}
