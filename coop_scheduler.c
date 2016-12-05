/*
The MIT License (MIT)

Copyright (c) 2014, Stephen Scott
All rights reserved.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

*/

#include "coop_scheduler.h"

// define interrupt enable/disable macros here if not defined elsewhere
// INTERRUPT_DECL     - allocate space for, and retrieve, current interrupt status
// DISABLE_INTERRUPTS - processor specific interrupt disable
// RESTORE_INTERRUPTS - processor specific restore previous interrupt status

// Macros are currently defined for ARM Cortex M0+, assume privilege mode enabled
void	__disable_irq	(void); 			// ARM Cortex-M CMSIS-CORE intrinsic function definition
uint32_t __get_PRIMASK	(void);				// ARM Cortex-M CMSIS-CORE intrinsic function definition
void	__set_PRIMASK	(uint32_t priMask);	// ARM Cortex-M CMSIS-CORE intrinsic function definition

#define INTERRUPT_DECL	int	interruptStatus = __get_PRIMASK();

#define DISABLE_INTERRUPTS				\
{										\
	__disable_irq();					\
}

#define RESTORE_INTERRUPTS				\
{										\
	__set_PRIMASK(interruptStatus);		\
}

// Instantiate Task Control Blocks here
// Begin TCB instantiation

// End TCB instantiation

// Define Task List here. Tasks are executed in round robin order listed in this array.
CoopTaskControlBlock * coopTaskList[] = {
	
	// leave this last
	NULL
};

/**************************************************************************************************/
// Background main loop, never returns

void CoopScheduler(void)
{
	CoopTaskControlBlock **	pTcbList = coopTaskList;

	while(1)
	{
		if (NULL == *pTcbList)
			pTcbList = coopTaskList;

		if ((*pTcbList) && (*pTcbList)->pFcn)
		{
			if (0 == (*pTcbList)->sleepTicks)
				(*((*pTcbList)->pFcn))(*pTcbList);

			pTcbList++;
		}
	}

} // CoopScheduler

/**************************************************************************************************/
// Yield to the next task.
void CoopYield(CoopTaskControlBlock * pTCB, CoopEntryFcn pFcn, void * pMsg, uint16_t entryPoint, uint32_t sleepTicks)
{
	INTERRUPT_DECL

	pTCB->pFcn = pFcn;
	pTCB->pMsg = pMsg;
	pTCB->entryPoint = entryPoint;

	DISABLE_INTERRUPTS	// to prevent timer interrupt from modifying sleep ticks while we mess with it
	pTCB->sleepTicks = sleepTicks;
	RESTORE_INTERRUPTS

} // CoopYield

/**************************************************************************************************/
// Must be called from Tick Timer ISR
void CoopTickTimer(void)
{
	CoopTaskControlBlock **	pTcbList = coopTaskList;

	while(NULL != *pTcbList)
	{
		if ((*pTcbList)->sleepTicks)
			(*pTcbList)->sleepTicks--;
		
		pTcbList++;
	}

}

