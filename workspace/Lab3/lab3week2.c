//Allison Yaeger 
//14244528

//libraries 
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
//header files 
#include "ece4220lab3.h"

#define RL 2			//pin number for the red led 
#define GL 4			//pin number for the green led 
#define YL 3			//pin number for the yellow led 
#define BUTTON 16		//pin number for the psuh button 
#define GLP 53			//prioriy for the green light 
#define RLP 53			//priority for the red light 
#define YLP 54			//priority for the yellow light 
#define STIME 100		//time in microseconds we want to sleep for after releasing the semaphore 
#define PER 4000000		//period time in microseconds 
#define INIT_VALUE 1		//initial value to use when initializing our semphore 

//our semaphore to use in the threads 
sem_t sem;

void *greenl(void *ptr);		//thread function to control the green light 
void *yellowl(void *ptr);		//thread function to control the yellow light 
void *redl(void *ptr);			//thread function to control the red pedestrian light 

typedef struct arguments{		//structure to pass arguments to our thread 
	int pinnum;
	int priority;
	int type;
}args;


int main(int argc, char* argv[]){

	if(argc != 4){		//check that the user entered the correct input arguments 
		printf("\nIncorrect Usage\nCorrect Usage: ./a.out <Priority PTL1> <priority PTL2> <priority PPL>\n");
		return -1;
	}

	//setup function 
	wiringPiSetupGpio();

	//define pin modes for the LEDs and buttons we will be using during this lab 
	pinMode(RL, OUTPUT);
	pinMode(GL, OUTPUT);
	pinMode(YL, OUTPUT);
	pinMode(BUTTON, INPUT);

	//make sure all lights are turned off 
	digitalWrite(RL, 0);
	digitalWrite(YL, 0);
	digitalWrite(GL, 0);

	//create arguments to send to the threads 
	args red, yellow, green;
	red.pinnum = RL;
	red.priority = atoi(argv[3]);
	yellow.pinnum = YL;
	yellow.priority = atoi(argv[1]);
	green.pinnum = GL;
	green.priority = atoi(argv[2]);

	//create threads
	sem_init(&sem, 0, INIT_VALUE);	
 	pthread_t rt, yt, gt;
	pthread_create(&rt, NULL, (void *)&redl, (void *)&red);
	pthread_create(&yt, NULL, (void *)&greenl, (void *)&yellow);
	pthread_create(&gt, NULL, (void *)&greenl, (void *)&green);

	pthread_join(rt, NULL);
	pthread_join(yt, NULL);
		
	return 0;
}

//thread function to control the green traffic light 
void *greenl(void *ptr){

	args *data = (args *)(ptr);				//typecast our void pointer so we can access it in the thread 

	struct sched_param param;				//set up shceduler and elevate priority with the given number 
	param.sched_priority = data->priority;
	sched_setscheduler(0, SCHED_FIFO, &param);
	uint64_t num_periods = 0;

	while(1){ 						//enter inifnite while loop to control the light 
		sem_wait(&sem);					//wait for access to the semaphore 
		digitalWrite(data->pinnum, 1);			//then turn the light on 
		usleep(PER);					//sleep for our period 
		digitalWrite(data->pinnum, 0);			//turn the light off 
		sem_post(&sem);					//release the semaphore 
		usleep(STIME);
	}

	pthread_exit(0);
}

//thread to control the red pedestrian light 
void *redl(void *ptr){
	args *data = (args *)(ptr);				//typecase the void pointer to so we can access it 	

	struct sched_param param;				//set up scheduler and elevate priority 
	param.sched_priority = data->priority;
	sched_setscheduler(0, SCHED_FIFO, &param);
	uint64_t num_periods = 0;
	clear_button();						//clear button to make sure it doesnt turn on at the begining 

	while(1){						//enter inifinite while loop tpo control the pedestrian light 
		if(check_button()){				//first we want to check if the button has been pressed 
			sem_wait(&sem);				//if so we wait fo rthe semaphore to be realeased 
			digitalWrite(data->pinnum, 1);		//then turn on the pedestrian light 
			usleep(PER);				//sleep for our period 
			digitalWrite(data->pinnum, 0);		//turn off the light 
			sem_post(&sem);				//release the sempahore 
			usleep(STIME);				//sleep for a small time 
			clear_button();				//then clear button so we can check it once again 
		}
	}		

	pthread_exit(0);
}
