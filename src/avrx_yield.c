#include "avrx.h"
#include "avrxcore.h"

void AvrXYield(void)
{
   _avrxBeginCritical();
   uint8_t *pUserContext;
   AVRXENTERKERNEL(pUserContext);
   pProcessID pSelf = _avrxAvrXKernelData.RunQueue->next;
   _avrxRemoveObject(_avrxAvrXKernelData.RunQueue, pSelf); 
   _avrxEndCritical();

   _avrxQueuePid(pSelf);
   _Epilog();
}
