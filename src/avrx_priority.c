#include "avrxkernel.h"

uint8_t AvrXTaskSetPriority(avrxPID p, uint8_t priority)
{
	uint8_t t = p->priority;
	p->priority = priority;
	return t;
}

uint8_t AvrXTaskGetPriority(avrxPID p)
{
	return p->priority;
}
