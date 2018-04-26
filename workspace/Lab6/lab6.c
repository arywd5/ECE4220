//Allison Yaeger 	
//14244528

#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kthread.h>	// for kthreads
#include <linux/sched.h>	// for task_struct
#include <linux/time.h>		// for using jiffies 
#include <linux/timer.h>
#include <linux/delay.h>
#include <linux/hrtimer.h>
#include <linux/ktime.h>
#include <linux/gpio.h>
#include <asm/io.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/interrupt.h>
#include <linux/delay.h>

MODULE_LICENSE("GPL");
int mydev_id;					//handler indentfier
unsigned long *ptr;				//pointer to memory map
static struct task_struct *kthread1;		//structure for our kthread 

//ISR function 
static irqreturn_t button_isr(int irq, void *dev_id){

	disable_irq_nosync(79);




	
	printk("\nInterrupt handled");
	enable_irq(79);	

	return IRQ_HANDLED;
}
//thread function 
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

int init_module(void){
	int dummy = 0;
	unsigned long *pullDown;
	ptr = (unsigned long *)ioremap(0x3f200000, 4096);		
	pullDown = ptr + 37;

	//set buttons as inputs and speaker as an output
	*ptr = *ptr | 0x00040044;			//set speaker as output 
	*(ptr + 1) = *(ptr + 1) | 0x000000000;		//set buttons as inputs 
	*(ptr + 2) = *(ptr + 2) | 0x000000000;
	*pulldown = *pullDown | 0x155;			//configure pull up down control  

	char kthread_name[11] = "my_kthread";
	
	printk("\nIn init module");

	dummy = request_irq(79, button_isr, IRQF_SHARED, "Button_handler", &mydev_id);

	kthread1 = kthread_create(kthread_fn, NULL, kthread_name);
	if((kthread1)){
		printk("\nInside if");
		wake_up_process(kthread1);
	}


	return 0;

}

void cleanup_module(void){

	free_irq(79, &mydev_id);

	printk("\nButton DEtection disabled.");
} 
