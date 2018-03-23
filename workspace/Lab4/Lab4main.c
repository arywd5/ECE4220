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
#include <math.h>
#include <sys/timerfd.h>
#include <semaphore.h>
#include <sys/types.h>
#include "ece4220lab3.h"


#define BUTTON 16
#define PERIOD 75000000
#define PRIORITY 55

typedef struct{
	int *flag;
	int *gps;
	struct timespec *t;
	struct timespec hit;
	int *simple;
} data;

typedef struct{
	double actual;
	uint64_t bt, at, pt;
	int before, after;
} printArgs;

void *thread0(void *num);                       //thread function to print values to the screen after reading from N
void *interpolate(void *ptr);			//this thread is called during interpolation process through thread0
void *print(void *ptr);				//function to print data through a simple pipe



int main(){

	//declare variables
	int N_pipe1, N_pipe2, number;
	int dummy, f, simp[2];
	pthread_t t0, s0;;
	struct timespec gpsT;
	data *buffer;								//declare buffer 
	buffer = (data *)malloc(sizeof(data *));					//malloc space for the buffer 
	buffer->gps = &number;							//set pointers equal to variables 
	buffer->t = &gpsT;
	buffer->flag = &f;

	if(pipe(simp) < 0){
		printf("\nError Creating simple pipe");
		exit(-1);
	}

	buffer->simple = &(simp[1]);

//	pthread_create(&s0, NULL, (void *)print, (void *)&(simp[0]));
	pthread_create(&t0, NULL, (void *)thread0, (void *)buffer);
	if((N_pipe1 = open("/tmp/N_pipe1", O_RDWR)) < 0){                     //open pipe
		printf("N_pipe1 error\n");                                      //if the pipe did not open properly exit
		exit(-1);
	}

	while(1){                                                               //while loop to read in data from the GPS_device
		if(read(N_pipe1, &number, sizeof(number)) < 0){         //read in pipe and store to buffer
			printf("\nN_pipe1 reading error\n");
			exit(-1);
		}
		else{	
			//	printf("\n%p -> %d", &number, number);	
			clock_gettime(CLOCK_MONOTONIC, &gpsT);	
			f = 1;
			usleep(1000);
		}
	}
	pthread_join(t0, NULL);
//	pthread_join(s0, NULL);

	return 0;
}

void *thread0(void *num){
	data *buffer = (data *)num;
	int prev = *(((data *)num)->gps);
	int *f = ((data *)num)->flag;	
	int dummy;
	struct timespec hittime;

	int N_pipe2;
	N_pipe2 = open("N_pipe2", O_RDONLY);
	if(N_pipe2 < 0){
		printf("\nN_pipe 2 could not be opened please try again");
		exit(0);
	}
	while(1){
		printf("\nWaiting for a button press....");		
		if(read(N_pipe2, &(hittime), sizeof(hittime)) != sizeof(hittime)){
			printf("\nError Reading in from N_pipe2 in thread0");
			exit(-1);	
		}
		else{
			((data *)num)->hit.tv_sec = hittime.tv_sec;
			((data *)num)->hit.tv_nsec = hittime.tv_nsec;

			pthread_t new;
			pthread_create(&new, NULL, (void *)interpolate, num);

		}
	}

	/*	while(1){
		printf("\n\n%p -> %d at time %ld", ((data *)num)->gps, *(((data *)num)->gps), (*((data *)num)->t));
		usleep(250000);
		}
	 */

	pthread_exit(0);
}

void *interpolate(void *num){

	printArgs curr;		
	uint64_t oldT, newT, actT;
	double slope, intercept;
	oldT = ( ((uint64_t)((*((data *)num)->t).tv_sec) * 1000000000)  + (uint64_t)(*((data *)num)->t).tv_nsec);
	actT =  ( ((uint64_t)(((data *)num)->hit.tv_sec) * 1000000000)  + (uint64_t)((data *)num)->hit.tv_nsec);
//	oldT = (*((data *)num)->t).tv_nsec;
	curr.before = *(((data *)num)->gps);




	while(curr.before == *(((data *)num)->gps)){
		usleep(75000);
	}

	newT = ( ((uint64_t)((*((data *)num)->t).tv_sec) * 1000000000)   +  (uint64_t)(*((data *)num)->t).tv_nsec);
	//newT = (*((data *)num)->t).tv_nsec;
	curr.after = *(((data *)num)->gps);

	//time for interpolation 


	long timediff = (long)(newT - oldT);
	double gpsdiff = (double)(curr.after - curr.before);
	slope = (double)(gpsdiff) / (double)timediff;
	intercept =(double)((double)(curr.after) - (slope*(double)newT));
	curr.actual = (double)((slope*(double)(actT)) + intercept);

	curr.bt = oldT;
	curr.at = newT;
	curr.pt = actT;

//	if( (write(*(((data *)num)->simple), &curr, sizeof(curr))) < 0){
//		printf("\nSimple pipe write unsucessfull");
//	}
//	else{
//		printf("\nsucessfully wrote to the simple pipe");
//	}

	printf("\n----------------Button Pressed-------------------");
	printf("\n\nBefore: %d at %lf", curr.before, oldT);
	printf("\nCalculated: %lf at %lf", curr.actual, actT);
	printf("\nAfter: %d at %lf\n\n", curr.after, newT);


	pthread_exit(0);
}
void *print(void *ptr){
	printArgs x;	

	while(1){

		if((read(*((int *)ptr), &x, sizeof(x))) != sizeof(x)){
			printf("\nError Reading in from pipe in print thread...");
			exit(0);
		}

		printf("\n----------------Button Pressed-------------------");
		printf("\n\nBefore: %d at %ld", x.before, x.bt);
		printf("\nCalculated: %lf at %ld", x.actual, x.pt);
		printf("\nAfter: %d at %ld\n\n", x.after, x.at);


	}

	pthread_exit(0);
}
