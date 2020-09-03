/* yield.c - yield */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  yield  -  Voluntarily relinquish the CPU (end a timeslice)
 *------------------------------------------------------------------------
 */
syscall	yield(void)
{
	intmask	mask;			/* Saved interrupt mask		*/
	uint32 start_low, start_high, stop_low, stop_high;
	uint64 start,stop;		
	struct	procent *prptr1=&proctab[currpid];
	
	mask = disable();
	prptr1->yield_c++;
	asm volatile ("CPUID\n\t""RDTSC\n\t""mov %%edx, %0\n\t""mov %%eax, %1\n\t":"=r" (start_high),"=r" (start_low)::"%rax","%rbx","%rcx","%rdx");	

	resched();
	
	asm volatile ("RDTSCP\n\t""mov %%edx, %0\n\t""mov %%eax, %1\n\t""CPUID\n\t":"=r" (start_high),"=r" (start_low)::"%rax","%rbx","%rcx","%rdx");
	
	start=((start_high)<<32)|start_low;
	stop=((stop_high)<<32)|stop_low;
	prptr1->yield_cycle=prptr1->yield_cycle+stop-start;	
		
	restore(mask);
	return OK;
}
