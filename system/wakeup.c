/* wakeup.c - wakeup */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  wakeup  -  Called by clock interrupt handler to awaken processes
 *------------------------------------------------------------------------
 */
void	wakeup(void)
{
	/* Awaken all processes that have no more time to sleep */

	uint32 start_low, start_high, stop_low, stop_high;
	uint64 start,stop;
	struct	procent *prptr1;
	prptr1=	&proctab[currpid];
	prptr1->wakeup_c++;
	asm volatile ("CPUID\n\t""RDTSC\n\t""mov %%edx, %0\n\t""mov %%eax, %1\n\t":"=r" (start_high),"=r" (start_low)::"%rax","%rbx","%rcx","%rdx");		
	
	resched_cntl(DEFER_START);
	while (nonempty(sleepq) && (firstkey(sleepq) <= 0)) {
		ready(dequeue(sleepq));
	}

	resched_cntl(DEFER_STOP);
	
	asm volatile ("RDTSCP\n\t""mov %%edx, %0\n\t""mov %%eax, %1\n\t""CPUID\n\t":"=r" (stop_high),"=r" (stop_low)::"%rax","%rbx","%rcx","%rdx");
	
	start=((uint64)(start_high)<<32)|start_low;
	stop=((uint64)(stop_high)<<32)|stop_low;
	prptr1->wakeup_cycle=prptr1->wakeup_cycle+stop-start;	
	return;
}
