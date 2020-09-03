// pr status syscall

#include<xinu.h>


void pr_status_syscall_summary()
{
	int32 i=0;
	struct	procent	*prptr;		/* pointer to process		*/	
	printf("\n%3s %7s %5s %13s\n","pid","syscall","count","average cycles");
	printf("%3s %7s %5s %7s %6s\n","---","-------","-----","--------------");
	for(i=0;i<NPROC;i++)
	{	
		prptr = &proctab[i];
		if(((prptr->create_c!=0)||(prptr->kill_c!=0)||(prptr->ready_c!=0)||(prptr->suspend_c!=0)||(prptr->yield_c!=0)||(prptr->sleep_c!=0)||(prptr->wait_c!=0)||(prptr->wakeup_c!=0))&&prptr->prstate!=PR_FREE)
		{
			if(prptr->create_c!=0)
			{
				printf("%3d %7s %5d %d \n",i,"create",prptr->create_c,(uint32)(prptr->create_cycle)/(prptr->create_c));		
			}
			if(prptr->kill_c!=0)
			{
				printf("%3d %7s %5d %d \n",i,"kill",prptr->kill_c,(uint32)(prptr->kill_cycle)/(prptr->kill_c));		
			}
			if(prptr->ready_c!=0)
			{
				printf("%3d %7s %5d %d \n",i,"ready",prptr->ready_c,(uint32)(prptr->ready_cycle)/(prptr->ready_c));		
			}
			if(prptr->sleep_c!=0)
			{
				printf("%3d %7s %5d %d \n",i,"sleepms",prptr->sleep_c,(uint32)(prptr->sleep_cycle)/(prptr->sleep_c));		
			}
			if(prptr->suspend_c!=0)
			{
				printf("%3d %7s %5d %d \n",i,"suspend",prptr->suspend_c,(uint32)(prptr->suspend_cycle)/(prptr->suspend_c));		
			}
			if(prptr->wait_c!=0)
			{
				printf("%3d %7s %5d %d \n",i,"wait",prptr->wait_c,(uint32)(prptr->wait_cycle)/(prptr->wait_c));		
			}
			if(prptr->wakeup_c!=0)
			{
				printf("%3d %7s %5d %d \n",i,"wakeup",prptr->wakeup_c,(uint32)(prptr->wakeup_cycle)/prptr->wakeup_c);		
			}
			if(prptr->yield_c!=0)
			{
				printf("%3d %7s %5d %d \n",i,"yield",prptr->yield_c,(uint32)(prptr->yield_cycle)/(prptr->yield_c));		
			}
			printf("%s\n","--------------------------------");
				
		}	

	}

}
