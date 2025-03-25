#include "avrxkernel.h"

avrxPID AvrXSelf(void)
{
   return _avrxKernelData.RunQueue;
}
