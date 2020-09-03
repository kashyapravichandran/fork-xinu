/* kill.c - kill */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  kill  -  Kill a process and remove it from the system
 *------------------------------------------------------------------------
 */
syscall	kill(
	  pid32		pid		/* ID of process to kill	*/
	)
{
	intmask	mask;			/* Saved interrupt mask		*/
	struct	procent *prptr, *prptr1;		/* Ptr to process's table entry	*/
	int32	i;			/* Index into descriptors	*/
	uint32 start_low, start_high, stop_low, stop_high;
	uint64 start,stop;
	mask = disable();
	
	prptr1 = &proctab[currpid];
	prptr1->kill_c++;
	asm volatile ("CPUID\n\t""RDTSC\n\t""mov %%edx, %0\n\t""mov %%eax, %1\n\t":"=r" (start_high),"=r" (start_low)::"%rax","%rbx","%rcx","%rdx");
	
	if (isbadpid(pid) || (pid == NULLPROC)
	    || ((prptr = &proctab[pid])->prstate) == PR_FREE) {
		
		asm volatile ("RDTSCP\n\t""mov %%edx, %0\n\t""mov %%eax, %1\n\t""CPUID\n\t":"=r" (stop_high),"=r" (stop_low)::"%rax","%rbx","%rcx","%rdx");
	
		start=((uint64)(start_high)<<32)|start_low;
		stop=((uint64)(stop_high)<<32)|stop_low;
		prptr1->kill_cycle=prptr1->kill_cycle+stop-start;

		restore(mask);
		return SYSERR;
	}

	if (--prcount <= 1) {		/* Last user process completes	*/
		xdone();
	}

	send(prptr->prparent, pid);
	for (i=0; i<3; i++) {
		close(prptr->prdesc[i]);
	}
	freestk(prptr->prstkbase, prptr->prstklen);

	switch (prptr->prstate) {
	case PR_CURR:
		prptr->prstate = PR_FREE;	/* Suicide */
		resched();

	case PR_SLEEP:
	case PR_RECTIM:
		unsleep(pid);
		prptr->prstate = PR_FREE;
		break;

	case PR_WAIT:
		semtab[prptr->prsem].scount++;
		/* Fall through */

	case PR_READY:
		getitem(pid);		/* Remove from queue */
		/* Fall through */

	default:
		prptr->prstate = PR_FREE;
	}
	
	asm volatile ("RDTSCP\n\t""mov %%edx, %0\n\t""mov %%eax, %1\n\t""CPUID\n\t":"=r" (stop_high),"=r" (stop_low)::"%rax","%rbx","%rcx","%rdx");
	
	start=((uint64)(start_high)<<32)|start_low;
	stop=((uint64)(stop_high)<<32)|stop_low;
	prptr1->kill_cycle=prptr1->kill_cycle+stop-start;
	restore(mask);
	return OK;
}
