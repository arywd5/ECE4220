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
	int *simple[1];
} data;

typedef struct{
	double actual;
	struct timespec bt, at, pt;
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
//	buffer->simple = &(simp[1]);

//	pthread_create(&s0, NULL, (void *)print, (void *)&simp);
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
		printf("\nN_pipe 2 could not be opened please tru again");
		exit(0);
	}
	while(1){		
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
	uint64_t oldT, newT;
	double slope, intercept;
//	oldT = ( ((*((data *)num)->t).tv_sec * 1000000000)  + (*((data *)num)->t).tv_nsec);
	oldT = (*((data *)num)->t).tv_nsec;
	curr.before = *(((data *)num)->gps);
	



	while(curr.before == *(((data *)num)->gps)){
		usleep(75000);
	}

//	newT = ( ((*((data *)num)->t).tv_sec * 1000000000)   +  (*((data *)num)->t).tv_nsec);
	newT = (*((data *)num)->t).tv_nsec;
	curr.after = *(((data *)num)->gps);

	//time for interpolation 
	

	slope = (double)(abs(((double)curr.after - (double)curr.before))/abs(newT - oldT));
	intercept =(double)((double)(newT) - (slope*(double)curr.after));
	curr.actual = (double)((slope*(((data *)num)->hit.tv_nsec)) + intercept);
	printf("\nInterpolated data = %lf from %d and %d", curr.actual, curr.before, curr.after);

	curr.bt.tv_nsec = oldT;
	curr.at.tv_nsec = newT;
	curr.pt.tv_nsec = (((data *)num)->hit.tv_nsec);

//	write(((data *)num)->simple, &curr, sizeof(curr));


	printf("\n----------------Button Pressed-------------------");
        printf("\n\nBefore: %d at %ld", curr.before, oldT);
        printf("\nCalculated: %d at %ld", curr.actual, ((data *)num)->hit.tv_nsec);
        printf("\nAfter: %d at %ld\n\n", curr.after, newT);
		
	

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
        printf("\n\nBefore: %d at %ld", x.before, x.bt.tv_nsec);
        printf("\nCalculated: %d at %ld", x.actual, x.pt.tv_nsec);
        printf("\nAfter: %d at %ld\n\n", x.after, x.at.tv_nsec);


	}

	pthread_exit(0);
}
