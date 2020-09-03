/* ready.c - ready */

#include <xinu.h>

qid16	readylist;			/* Index of ready list		*/

/*------------------------------------------------------------------------
 *  ready  -  Make a process eligible for CPU service
 *------------------------------------------------------------------------
 */
status	ready(
	  pid32		pid		/* ID of process to make ready	*/
	)
{
	register struct procent *prptr, *prptr1;
	uint32 start_low, start_high, stop_low, stop_high;
	uint64 start,stop;	
	prptr1 = &proctab[currpid];
	prptr1->ready_c++;
	asm volatile ("CPUID\n\t""RDTSC\n\t""mov %%edx, %0\n\t""mov %%eax, %1\n\t":"=r" (start_high),"=r" (start_low)::"%rax","%rbx","%rcx","%rdx");

	if (isbadpid(pid)) {
		
		asm volatile ("RDTSCP\n\t""mov %%edx, %0\n\t""mov %%eax, %1\n\t""CPUID\n\t":"=r" (stop_high),"=r" (stop_low)::"%rax","%rbx","%rcx","%rdx");
	
		start=((uint64)(start_high)<<32)|start_low;
		stop=((uint64)(stop_high)<<32)|stop_low;
		prptr1->ready_cycle=prptr1->ready_cycle+stop-start;
		return SYSERR;
	}

	/* Set process state to indicate ready and add to ready list */

	prptr = &proctab[pid];
	prptr->prstate = PR_READY;
	insert(pid, readylist, prptr->prprio);
	resched();
	
	asm volatile ("RDTSCP\n\t""mov %%edx, %0\n\t""mov %%eax, %1\n\t""CPUID\n\t":"=r" (stop_high),"=r" (stop_low)::"%rax","%rbx","%rcx","%rdx");
	
	start=((uint64)(start_high)<<32)|start_low;
	stop=((uint64)(stop_high)<<32)|stop_low;
	prptr1->ready_cycle=prptr1->ready_cycle+stop-start;

	return OK;
}
