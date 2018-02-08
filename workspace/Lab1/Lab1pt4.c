//Allison Yaeger 
//14244528

#ifndef __KERNEL__
#define __KERNEL__
#endif

#ifdef MODULE
#define MODULE
#endif

#include <linux/module.h>
#include <linux/kernel.h>
#include <asm/io.h>

int init_module(void){
	unsigned long *ptr;
	ptr = (unsigned long *)ioremap(0x3F200000, 4096);

	//set pins as outputs using bitmasking 
	*ptr = *ptr | 0x09240;
	
	//get to pin output
	ptr = ptr + 7;
	*ptr = *ptr | 0x3C;
	
	printk("MODULE INSTALLED");
	
	return 0;
}

void cleanup_module(void){

	unsigned long *ptr;
	ptr = (unsigned long *)ioremap(0x3f200000, 4096);

	*ptr = *ptr | 0x09240;
	
	ptr = ptr + 10;
	*ptr = *ptr | 0x03C;                                      

	printk("MODULE REMOVED");
}

