//Allison Yaeger 
//14244528

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <wiringPi.h>
#include <unistd.h>
#include <fcntl.h>
#include <sched.h>
#include <stdint.h>
#include <sys/timerfd.h>
#include <semaphore.h>

#define BUTTON 16
#define PERIOD 75000000

void  *thread0(void *num);



int main(){
	
	//map the ports and declare button as an input 
	wiringPiSetupGpio();
	pinMode(BUTTON, INPUT);
	
	//declare pipe variables 
	int N_pipe1, N_pipe2;
	int dummy, buffer;
	//create the pipes 
//	dummy = system("mkfifo N_pipe1");
//	dummy = system("mkfifo N_pipe2");
	//open the pipes 
	if((N_pipe1 = open("/tmp/N_pipe1", O_RDONLY)) < 0){
		printf("N_pipe1 error\n");
		exit(-1);
	}
//	if((N_pipe2 = open("N_pipe2", O_RDONLY)) < 0){
//		printf("N_pipe2 error\n");
//		exit(-1);
//	}
	
	pthread_t t0;
	pthread_create(&t0, NULL, (void *)thread0, (void *)&buffer);
	
	
	//while loop to read in data from the GPS_device
	while(1){
		if(read(N_pipe1, &buffer, sizeof(buffer)) < 0){
			printf("\nN_pipe1 reading error\n");
			exit(-1);
		}
//		printf("\n%d", buffer);
	}
	
	pthread_join(t0, NULL);	
	
	return 0;
}
//thread0 function that prints buffer to the screen to make sure its working properly 
void *thread0(void *num){
	
	while(1){
		printf("\nData = %d", *((int *)num));
		usleep(2500);
	}
	
	
	pthread_exit(0);
}
//second process that will check if the buton has been pressed 
void process2(int pipe){
	pthread_t t1;
	//create a thread to check for the button periodically 
	pthread_create(&t1, NULL, (void *)buttonThread, (void *)&pipe);	
	//join thread 
	pthread_join(t1, NULL);

}
//thread function to check wether the button has been pressed 
void *buttonThread(void *ptr){
	int pipe = *((int *)ptr);
	int timer = timerfd_create(CLOCK_MONOTONIC, 0);
	struct itimerspec itval;

	itval.it_value.tv_sec = 0;
	itval.it_value.tv_nsec = 0;
	
	itval.it_interval.tv_nsec = 0;
	itval.it_interval.tv_nsec = PERIOD;

	

	pthread_exit(0);
}
