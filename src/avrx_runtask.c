#include "avrxkernel.h"

void AvrXRunTask(uint8_t *pStack,
                 void(*pTask)(void),
                 avrxPID pid,
                 uint8_t priority)
{
   avrxPID pPID = AvrXTaskInit(pStack, pTask, pid, priority);
   AvrXResume(pPID);
}
