//Allison Yaeger 
//14244528

//libraries 
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <lwiringpi.h>
#include <unistd.h>
#include <fdntl.h>
#include <sched.h>
#include <stdint.h>
#include <sys/timerfd.h>
//header files 
#include "ece4220lab3.h"

#define RL 2			//pin number for the red led 
#define GL 4			//pin number for the green led 
#define YL 3			//pin number for the yellow led 
#define BUTTON 16		//pin number for the psuh button 
#define GLP 51			//prioriy for the green light 
#define RLP 51			//priority for the red light 
#define YLP 51			//priority for the yellow light 



void *greenl(void *ptr);		//thread function to control the green light 
void *yellowl(void *ptr);		//thread function to control the yellow light 
void *redl(void *ptr);			//thread function to control the red pedestrian light 

typedef struct arguments{
	int waitnsec;
	int pinnum;
	int priority;
	long nperiod;
	
}args;


int main(int argc, char* argv[]){

	if(argc != 2){		//check that the user entered the correct input arguments 
		printf("\nIncorrect Usage\nCorrect Usage: ./a.out <policy>\nFIFO - 1, RR - 2");
		return -1;
	}

	//setup function 
	wiringPiSetupGpio();

	//define pin modes for the LEDs and buttons we will be using during this lab 
	pinMode(RL, OUTPUT);
	pinMode(GL, OUTPUT);
	pinMode(YL, OUTPUT);
	pinMode(BUTTON, INPUT);

	//create infinite loop 
	while(1){

		if(digitalRead(BUTTON)){
			

		}
	





	}

	return 0;
}
/*
//thread function to control the green traffic light 
void *greenl(void *ptr){
	
	args *data = (args *)(ptr);
	
	//create timer 
	int timer = timerfd_create(CLOCK_MONOTONIC, 0);
	struct itimerspec itval;

	//set up timer 
	itval.it_value.tv_sec = 0;
	itval.it_value.tv_nsec = data->waitnsec;
	
	//set up period 
	itval.it_interval.tv_sec = 0;
	itval.it_interval.tv_nsec = data->nperiod;
	
	struct sched_param param;
	param.sched_priority = data->priority;
	sched_setscheduler(0, SCHED_FIFO, &param);
	uint64_t num_periods = 0;
	
	while(1){ 
		
		
		
	}

	pthread_exit(0);
}

//thread to control the red pedestrian light 
void *redl(){
	args *data = (args *)(ptr);
		
		//create timer 
		int timer = timerfd_create(CLOCK_MONOTONIC, 0);
		struct itimerspec itval;

		//set up timer 
		itval.it_value.tv_sec = 0;
		itval.it_value.tv_nsec = data->waitnsec;
		
		//set up period 
		itval.it_interval.tv_sec = 0;
		itval.it_interval.tv_nsec = data->nperiod;
		
	struct sched_param param;
	param.sched_priority = data->priority;
	sched_setscheduler(0, SCHED_FIFO, &param);
	uint64_t num_periods = 0;
		
	while	
		
	
	pthread_exit(0);
}*/
