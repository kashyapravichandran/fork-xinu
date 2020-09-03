/* fork.c - forking from a parent thread */ 

#include<xinu.h>

local int newpid();


// Function to fork of the parent function

pid32 fork()

{
	
	int32		savsp, *pushsp;
	intmask 	mask;    	/* Interrupt mask		*/
	pid32		pid;		/* Stores new process id	*/
	struct	procent	*prptr,	*prptrcur;	/* Pointer to proc. table entry */
	int32		i;
	uint32		*a;		/* Points to list of args	*/
	uint32		*saddr;		/* Stack address		*/
	//uint32  	stack_value; 	/* Calling address's stack pointer */
	unsigned long *sp, *fp, *bp;
	
	// Need to add stack stuff here. 
	// Store all the general purpose registers, 
	// Store the flags. 
	mask = disable();

	prptrcur = &proctab[currpid];
	if((prptrcur->prprio<1) || ((pid=newpid()) == SYSERR) || ((saddr = (uint32 *)getstk(prptrcur->prstklen)) == (uint32 *)SYSERR)){
	restore(mask);
	return SYSERR;
	}	
	
	prcount++;
	prptr = &proctab[pid];	
	prptr->prstate = PR_SUSP; 
	prptr->prprio = prptrcur->prprio;
	prptr->prstkbase = (char*)saddr;
	prptr->prstklen = prptrcur->prstklen;
	prptr->prname[PNMLEN-1] = NULLCH;
	for (i=0 ; i<PNMLEN-1 && (prptr->prname[i]=prptrcur->prname[i])!=NULLCH; i++)
		;
	prptr->prparent = currpid;
	prptr->prsem = -1;
	prptr->prhasmsg = FALSE;

	/* Set up stdin, stdout, and stderr descriptors for the shell	*/
		
	prptr->prdesc[0] = CONSOLE;
	prptr->prdesc[1] = CONSOLE;
	prptr->prdesc[2] = CONSOLE;
	savsp =(uint32)saddr;
	pushsp = (uint32) prptrcur->prstkbase;
	// stack stuff
	asm("movl %%esp, %0\n" :"=r"(sp));
	asm("movl %%ebp, %0\n" :"=r"(fp));
	bp = (unsigned long *) prptrcur->prstkbase;
	while(bp>=fp)
	{
		*saddr = *bp;
		if(*saddr==pushsp)
		{
			pushsp=bp;
			*saddr=savsp;
			savsp = saddr;
		}
		saddr--;
		bp--;		
	}
	// flag 
	*saddr = 0x00000200; // flag
	*--saddr = NPROC; // eax
	*--saddr = 0; 	//ecx
	*--saddr = 0;   //edx
	*--saddr = 0;	//ebx
	*--saddr = 0; 	//esp
	pushsp = saddr; 
	*--saddr = savsp; //ebp
	*--saddr = 0; //esi
	*--saddr = 0; //edi;
	*pushsp = (unsigned long) (prptr->prstkptr = (char *)saddr);

	//prptrcur->ready_c++;
	// putting stuff into the ready list 
	//prptr = &proctab[pid];
	prptr->prstate = PR_READY;
	insert(pid, readylist, prptr->prprio);
	restore(mask);
	
	return pid;	
}

// ---------------- New PID ----------------- code copied from create.c

local	pid32	newpid(void)
{
	uint32	i;			/* Iterate through all processes*/
	static	pid32 nextpid = 1;	/* Position in table to try or	*/
					/*   one beyond end of table	*/

	/* Check all NPROC slots */

	for (i = 0; i < NPROC; i++) {
		nextpid %= NPROC;	/* Wrap around to beginning */
		if (proctab[nextpid].prstate == PR_FREE) {
			return nextpid++;
		} else {
			nextpid++;
		}
	}
	return (pid32) SYSERR;
}

