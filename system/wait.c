/* wait.c - wait */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  wait  -  Cause current process to wait on a semaphore
 *------------------------------------------------------------------------
 */
syscall	wait(
	  sid32		sem		/* Semaphore on which to wait  */
	)
{
	intmask mask;			/* Saved interrupt mask		*/
	struct	procent *prptr,*prptr1;		/* Ptr to process's table entry	*/
	struct	sentry *semptr;		/* Ptr to sempahore table entry	*/
	
	uint32 start_low, start_high, stop_low, stop_high;
	uint64 start,stop;

	prptr1=	&proctab[currpid];
	//prptr1->wait_c++;
	
	mask = disable();
	
	prptr1->wait_c++;
	asm volatile ("CPUID\n\t""RDTSC\n\t""mov %%edx, %0\n\t""mov %%eax, %1\n\t":"=r" (start_high),"=r" (start_low)::"%rax","%rbx","%rcx","%rdx");	
	
	if (isbadsem(sem)) {
		
		asm volatile ("RDTSCP\n\t""mov %%edx, %0\n\t""mov %%eax, %1\n\t""CPUID\n\t":"=r" (stop_high),"=r" (stop_low)::"%rax","%rbx","%rcx","%rdx");
		start=((uint64)(start_high)<<32)|start_low;
		stop=((uint64)(stop_high)<<32)|stop_low;
		prptr1->wait_cycle=prptr1->wait_cycle+stop-start;	
			
		restore(mask);
		return SYSERR;
	}

	semptr = &semtab[sem];
	if (semptr->sstate == S_FREE) {
		
		asm volatile ("RDTSCP\n\t""mov %%edx, %0\n\t""mov %%eax, %1\n\t""CPUID\n\t":"=r" (stop_high),"=r" (stop_low)::"%rax","%rbx","%rcx","%rdx");
		start=((uint64)(start_high)<<32)|start_low;
		stop=((uint64)(stop_high)<<32)|stop_low;
		prptr1->wait_cycle=prptr1->wait_cycle+stop-start;	
		restore(mask);
		return SYSERR;
	}

	if (--(semptr->scount) < 0) {		/* If caller must block	*/
		prptr = &proctab[currpid];
		prptr->prstate = PR_WAIT;	/* Set state to waiting	*/
		prptr->prsem = sem;		/* Record semaphore ID	*/
		enqueue(currpid,semptr->squeue);/* Enqueue on semaphore	*/
		resched();			/*   and reschedule	*/
	}

	
	
	asm volatile ("RDTSCP\n\t""mov %%edx, %0\n\t""mov %%eax, %1\n\t""CPUID\n\t":"=r" (stop_high),"=r" (stop_low)::"%rax","%rbx","%rcx","%rdx");
	
	start=((uint64)(start_high)<<32)|start_low;
	stop=((uint64)(stop_high)<<32)|stop_low;
	prptr1->wait_cycle=prptr1->wait_cycle+stop-start;	
	restore(mask);	
	return OK;
}
