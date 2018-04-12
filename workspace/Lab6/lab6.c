//Allison Yaeger 	
//14244528

#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>
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



//function prototypes 
void *thread1(void *data); //thread function to create sound on the auxillary board 


int main(int argc, char *argv[]){
	
	if(argc != 2){
		printf("\nImproper Usage, example:\n./a.out <run type>");
		printf("Run Types:\n	1 -- Kthread\n	2 -- hrtimer");
		return -1;
	}	
	
	if(atoi(argv[1]) == 1){
		

	}

	else if(atoi(argv[1]) == 2){


	}

	else{
		printf("\nImproper run type, please try again");
		return -1;
	}


	return 0;
}
void *thread1(void *data){
	int freq = *((int *)data);	
	
	while(1){
		

	}

}


