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
int mydev_id;			//handler indentfier
unsigned long *ptr;		//pointer to memory map

static irqreturn_t button_isr(int irq, void *dev_id){

	disable_irq_nosync(79);




	
	printk("\nInterrupt handled");
	enable_irq(79);	

	return IRQ_HANDLED;
}

int init_module(void){
	int dummy = 0;
	ptr = (unsigned long *)ioremap(0x3f200000, 4096);
		
	//set buttons as inputs and speaker as an output
		


	dummy = request_irq(79, button_isr, IRQF_SHARED, "Button_handler", &mydev_id);

	return 0;

}

void cleanup_module(void){

	free_irq(79, &mydev_id);

	printk("\nButton DEtection disabled.");
} 
