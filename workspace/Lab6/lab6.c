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

MODULE_LICENSE("GPL");

unsigned long *ptr;

int init_module(void){
	ptr = (unsigned long *)ioremap(0x3f200000, 4096);
	
	//set buttons as inputs and speaker as an output
	



}

void cleanup_module(void){




} 
