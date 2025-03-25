/* Copyright (c) 2025 Neil Johnson */
#include "avrx.inc"
        _MODULE avrx_setkernelstack
        _FUNCTION AvrXSetKernelStack

/* void * AvrXSetKernelStack(void *pStack) */
AvrXSetKernelStack:
        pop     R31
        pop     R30
        subi    R24, 0
        sbci    R25, 0
        brne    sks1
        in      R24, _SFR_IO_ADDR(SPL)
        in      R25, _SFR_IO_ADDR(SPH)
sks1:
        sts     _avrxKernelData+AvrXStack+NextL, R24
        sts     _avrxKernelData+AvrXStack+NextH, R25
        ijmp

        _ENDFUNC AvrXSetKernelStack
