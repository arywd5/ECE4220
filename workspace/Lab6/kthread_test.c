/* Based on example from: http://tuxthink.blogspot.com/2011/02/kernel-thread-creation-1.html
   Modified and commented by: Luis Rivera			
   
   Compile using the Makefile
*/

#ifndef MODULE
#define MODULE
#endif
#ifndef __KERNEL__
#define __KERNEL__
#endif
   
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kthread.h>	// for kthreads
#include <linux/sched.h>	// for task_struct
#include <linux/time.h>		// for using jiffies 
#include <linux/timer.h>
#include <linux/delay.h>
#include <linux/gpio.h>
#include <asm/io.h>

MODULE_LICENSE("GPL");

// structure for the kthread.
static struct task_struct *kthread1;
unsigned long *ptr;

// Function to be associated with the kthread; what the kthread executes.
int kthread_fn(void *p)
{		
	printk("Before loop\n");
	
	while(1)
	{

		*(ptr + 7) = (*(ptr + 7)) | 0x00000044;			//turn speaker high 
		udelay(800);	// good for a few us (micro s) 		//delay for 800 microseconds 
		*(ptr + 10) = (*(ptr + 10)) | 0x00000044;		//turn speaker low 
		udelay(800);						//sleep for 800 microseconds 
		
		//check is we should exit the thread 
		if(kthread_should_stop()) {
			do_exit(0);
		}			
	}
	
	return 0;
}

int thread_init(void)
{
	ptr = (unsigned long *)ioremap(0x3f200000, 4096); 
	iowrite32( (*ptr | 0x00040040), ptr);			//set speaker as outputs after memory map is created 
	iowrite32( (*ptr | 0x00000044), ptr);


	char kthread_name[11]="my_kthread";	// try running  ps -ef | grep my_kthread
										// when the thread is active.
	printk("\nIn init module\n");
    	    
    kthread1 = kthread_create(kthread_fn, NULL, kthread_name);
	
    if((kthread1))	// true if kthread creation is successful
    {
        printk("Inside if\n");
		// kthread is dormant after creation. Needs to be woken up
        wake_up_process(kthread1);
    }

    return 0;
}

void thread_cleanup(void) {
	int ret;
	// the following doesn't actually stop the thread, but signals that
	// the thread should stop itself (with do_exit above).
	// kthread should not be called if the thread has already stopped.
	ret = kthread_stop(kthread1);
	
	gpio_free(16);
							
	if(!ret)
		printk("Kthread stopped\n");
}

// Notice this alternative way to define your init_module()
// and cleanup_module(). "thread_init" will execute when you install your
// module. "thread_cleanup" will execute when you remove your module.
// You can give different names to those functions.
module_init(thread_init);
module_exit(thread_cleanup);
