#include "avrxkernel.h"

avrxPID AvrXTaskInit(uint8_t *pStack,
                     void   (*pTask)(void),
                     avrxPID  pid,
                     uint8_t  priority)
{
   *pStack-- = ((uint16_t)pTask) & 0xFF;
   *pStack-- = (((uint16_t)pTask)>>8) & 0xFF;

   for (uint8_t i=2; i<_AVRX_MINCONTEXT; i++)
      *pStack-- = 0;

   pid->ContextPointer = (void *)pStack;
   pid->priority       = priority;
   pid->flags          = AVRX_PID_Suspend
                       | AVRX_PID_Suspended;
   pid->next           = 0;

   return pid;
}
