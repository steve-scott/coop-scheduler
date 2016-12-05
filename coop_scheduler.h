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

/** @file coop_scheduler.h
 * @brief cooperative round-robin multitasking scheduler implementation
 */

#ifndef _COOP_SCHEDULER_H_
#define _COOP_SCHEDULER_H_

#include <stdlib.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct CoopTaskControlBlock;
typedef struct CoopTaskControlBlock	CoopTaskControlBlock;
typedef void (*CoopEntryFcn)(CoopTaskControlBlock * pTCB);

struct CoopTaskControlBlock
{
	CoopEntryFcn	pFcn;		// pointer to reentry function
	void		  *	pMsg;		// pointer to message being processed
	uint32_t		sleepTicks;	// number of ticks to sleep before reentry
	uint16_t		entryPoint;	// identifies where to resume processing
};

#define COOP_TCB_INIT(fcn) { .pFcn = fcn, .pMsg=NULL, .sleepTicks=0, .entryPoint=0 }

/** Background main loop scheduler. Never returns.
 *
 * @returns                 none
 */
void CoopScheduler(void);

/** Tick timer handler. Call from tick timer ISR
 *
 * @returns                 none
 */
void CoopTickTimer(void);

/** Yield to the next task
 *
 * @param[in] pTCB          pointer to current task's TCB
 * @param[in] pFcn          pointer to function where processing is to resume
 * @param[in] pMsg          pointer to message being processed (can be NULL)
 * @param[in] sleepTicks    number of ticks to wait before reentry
 * @param[in] entryPoint    identifies (e.g. switch statement case id) where to resume processing
 * @returns                 none
 */
void CoopYield(CoopTaskControlBlock * pTCB, CoopEntryFcn pFcn, void * pMsg, uint16_t entryPoint, uint32_t sleepTicks);

#ifdef __cplusplus
}
#endif

#endif // _COOP_SCHEDULER_H_
