//Allison Yaeger 	
//14244528

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
#include <linux/hrtimer.h>
#include <linux/ktime.h>
#include <linux/gpio.h>
#include <asm/io.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/fs.h>
#include <asm/uaccess.h>

#define MSG_SIZE 40
#define CDEV_NAME "Lab6"

MODULE_LICENSE("GPL");
int mydev_id;					//handler indentfier
unsigned long *ptr;				//pointer to memory map
static struct task_struct *kthread1;		//structure for our kthread 
int frequency = 800;
static int major;
static char msg[MSG_SIZE];

//function to read in from character device 
static ssize_t device_read(struct file *filp, char __user *buffer, size_t length, loff_t *offset){
	
	ssize_t dummy = copy_to_user(buffer, msg, length);
	if(!dummy){
		switch(msg[0]){
			case 'A':
				frequency = 800;
				printk("%c recieved through chrdev freq set to 800\n", msg[0]);
				break;
			case 'B':
				frequency = 650;
				printk("%c recieved through chrdev freq set to 650\n", msg[0]);
				break;
			case 'C':
				frequency = 500;
				printk("%c recieved through chrdev freq set to 500\n", msg[0]);
				break;
			case 'D':
				frequency = 350;
				printk("%c recieved through chrdev freq set to 350\n", msg[0]);
				break;
			case 'E':
				frequency = 200;
				printk("%c recieved through chrdev freq set to 200\n", msg[0]);
				break;
			default:
				printk("%c recieved from character device\n", msg[0]);
				break;
		}			

	}
	else{
		printk("Message reading error\n");
	}

	return length;

}
//device write function 
static ssize_t device_write(struct file *filp, const char __user *buff, size_t len, loff_t *off){
	ssize_t dummy;

	if(len > MSG_SIZE)
		return -EINVAL;

	dummy = copy_from_user(msg, buff, len);
	if(len == MSG_SIZE)
		msg[len -1] = '\0';
	else
		msg[len] = '\0';

	switch(msg[0]){
		case 'A':
			frequency = 800;
			printk("%c recieved through chrdev freq set to 800\n", msg[0]);
			break;
		case 'B':
			frequency = 650;
			printk("%c recieved through chrdev freq set to 650\n", msg[0]);	
			break;
		case 'C':
			frequency = 500;
			printk("%c recieved through the chrdev freq set to 500\n", msg[0]);
			break;
		case 'D':
			frequency = 350;
			printk("%c recieved through the chrdev freq set to 350\n", msg[0]);
			break;
		case 'E':
			frequency = 200;
			printk("%c recieved through the chrdev freq set to 200\n", msg[0]);
			break;
		default:	
			printk("%c recieved through chrdev\n", msg[0]);
			break;
	}

	return len;
	
}

//ISR function 
static irqreturn_t button_isr(int irq, void *dev_id){

	disable_irq_nosync(79);
	unsigned long button = (unsigned long)(*(ptr + 16) & 0x1f0000);	//access the event detect register
								//and with 0x1f0000 to show which button has been pressed 
	switch(button){

		case 0x10000:			//button one was pressed 
			frequency = 800;
			printk("Button 1 pressed, frequency = 800\n");	
			break;
		case 0x20000:			//button two was pressed 
			frequency = 650;
			printk("Button 2 pressed, frequency = 650\n");	
			break;
		case 0x40000:			//button three was pressed 
			frequency = 500;
			printk("Button 3 pressed, frequency = 500\n");
			break;
		case 0x80000:			// button four was pressed 
			frequency = 350;
			printk("Button 4 pressed, frequency = 350\n");
			break;
		case 0x100000: 			//button five was pressed
			frequency = 200;
			printk("Button 5 pressed, frequency = 200\n");
			break;
	}
	*(ptr + 16) = *(ptr + 16) | 0x1f0000;	//clear register after a detection
	
	printk("Interrupt handled\n");
	enable_irq(79);	

	return IRQ_HANDLED;
}

//thread function 
int kthread_fn(void *p)
{		
	printk("\nBefore loop");
	
	while(1)
	{

		*(ptr + 7) = (*(ptr + 7)) | 0x0040;				//turn speaker high 
		udelay(frequency);	// good for a few us (micro s) 		//delay for 800 microseconds 
		*(ptr + 10) = (*(ptr + 10)) | 0x0040;				//turn speaker low 
		udelay(frequency);						//sleep for 800 microseconds 
		
		//check is we should exit the thread 
		if(kthread_should_stop()) {
			do_exit(0);
		}			
	}
	
	return 0;
}
static struct file_operations fops = {
	.read = device_read,
	.write = device_write,

};
int init_module(void){
	int dummy = 0;
	unsigned long *pullDown;
	ptr = (unsigned long *)ioremap(0x3f200000, 4096);		
	pullDown = ptr + 37;

	//request character device and obtain the major
	major = register_chrdev(0, CDEV_NAME, &fops);
	if(major <0){
		printk("REgistering the character device failed with %d\n", major);
		return major;
	}
	printk("Major number is: %d\n", major);
	//set buttons as inputs and speaker as an output
	*ptr = *ptr | 0x00040040;			//set speaker as output 
	*(ptr + 1) = *(ptr + 1) | 0x000000000;		//set buttons as inputs 
	*(ptr + 2) = *(ptr + 2) | 0x000000000;
	
	*pullDown = *pullDown | 0x155;			//configure pull up down control  
	udelay(100);					//delay for 150 clock cycles 
	
	*(pullDown + 1) = (*pullDown |  0x001f0000);	//
	udelay(100);					//delay again 

	*pullDown = *pullDown & (~0x155);		//set back to zero 
	*(pullDown + 1) = *(pullDown + 1) & (~0x001f0000);

	*(ptr + 31) = 	*(ptr + 31) | 0x1f0000;		//set rising edge detect

	char kthread_name[11] = "my_kthread";
	
	printk("\nIn init module");
	//create ISR
	dummy = request_irq(79, button_isr, IRQF_SHARED, "Button_handler", &mydev_id);
	//Create kthread
	kthread1 = kthread_create(kthread_fn, NULL, kthread_name);
	if((kthread1)){
		printk("\nInside if");
		wake_up_process(kthread1);
	}

	

	return 0;

}

void cleanup_module(void){
	int ret;
	*(ptr + 16) = *(ptr + 16) | 0x1f0000;	
	*(ptr + 31) = *(ptr + 31) & (~0x1f0000);

	free_irq(79, &mydev_id);

	ret = kthread_stop(kthread1);
	if(!ret)
		printk("\nKthread Stopped!");

	unregister_chrdev(major, CDEV_NAME);
	printk("\nButton DEtection disabled.");
} 
