#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/list.h>

static __init int print_proInfo(void)
{
	struct task_struct *initTask,*p;
	struct list_head* pos;

	int total=0;
	int running=0;
	int interruptible=0;
	int uninterruptible=0;
	int traced=0;
	int stopped=0;

	int zombie=0;
	int dead=0;

	int unknown=0;

	initTask=&init_task;
	printk("\nSTART TO PRINT\n");

	list_for_each(pos, &(initTask->tasks))
	{
		p=list_entry(pos, struct task_struct, tasks);
		total++;
		printk("%s pid:%d parent: %s parent's pid: %d ",p->comm,p->pid,p->real_parent->comm,p->real_parent->pid, p->nr_dirtied);

		switch(p->state)
		{
			case TASK_RUNNING:printk("state: RUNNING");running++;break;
			case TASK_INTERRUPTIBLE:printk("state: INTERRUPTIBLE");interruptible++;break;
			case TASK_UNINTERRUPTIBLE:printk("state: UNINTERRUPTIBLE");uninterruptible++;break;
			case TASK_TRACED:printk("state: TRACED");traced++;break;
			case TASK_STOPPED:printk("state: STOPPED");stopped++;break;
			case EXIT_ZOMBIE:zombie++;printk("exit_state: ZOMBIE");break;
			case EXIT_DEAD:dead++;printk("exit_state: DEAD");break;
			default:printk("state: UNKNOWN");unknown++;break;
		}

		printk("\n");
	}

	printk("total number of process is %d\n",total);
	printk("TASK_RUNNING:%d\n",running);
	printk("TASK_INTERRUPTIBLE:%d\n",interruptible);
	printk("TASK_UNONTERRUPTIBLE:%d\n",uninterruptible);
	printk("TASK_STOPPED:%d\n",stopped);
	printk("TASK_TRACED:%d\n",traced);
	printk("TASK_UNKNOWN:%d\n",unknown);

	return 0;
}

static __exit void print_exit(void)
{
	printk("<0>PRINT END\n");
}

module_init(print_proInfo);
module_exit(print_exit);

	// struct task_struct *initTask,*p;
	// struct list_head* pos;

	// initTask=&init_task;

	// list_for_each(pos, &(initTask->tasks))
	// {
	// 	p=list_entry(pos, struct task_struct, tasks);
	// 	printk("%s pid:%d the number of dirtied pages: %d",p->comm,p->pid,p->nr_dirtied);
	// }