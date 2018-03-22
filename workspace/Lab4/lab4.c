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
#include <sys/types.h>
#include "ece4220lab3.h"


#define BUTTON 16
#define PERIOD 75000000
#define PRIORITY 55

typedef struct{
	int *N_pipe2;
	sem_t *sem2;
	int *gpsdata;
	struct timespec *t;
	int simple[2];
	int *flag;
} buff;

typedef struct{                                 //structure to pass to threads with all the time information
	buff *buffer;
	struct timespec previous;
	struct timespec next;
	struct timespec hittime;
	int before;
	int after;
	int actual;
	int *simple;
} data;


void *thread0(void *num);			//thread function to print values to the screen after reading from N_pipe1
void process2(buff buffer);			//process to complete the second part of this lab 
void *buttonThread(void *ptr);			//thread to check if the button has been pressed each 75 ms
void *interpolate(void *ptr);			//thread to interpolate the data given 
void *print(void *ptr);				//thread that will read in data from a simple pipe and print it to user 

int main(){

	wiringPiSetupGpio();							//set up wiring Pi 
	pinMode(BUTTON, INPUT);							//declare our button as an input 
	pullUpDnControl(BUTTON, PUD_DOWN);


	//declare pipe variables 
	int N_pipe1, pipe2;
	int dummy, flag;
	int data, number;
	sem_t sem2;
	struct timespec clock;
	buff *buffer = malloc(sizeof(buffer));
	buffer->N_pipe2 = &pipe2;
	buffer->sem2 = &sem2;
	buffer->gpsdata = &data;
	buffer->t = &clock;
	buffer->flag = &flag;

	sem_init((buffer->sem2), 0, 1);						//initialize semaphore for pipe2
	system("mkfifo N_pipe2");						//create pipe 
	if((pipe2 = open("N_pipe2", O_RDWR)) < 0){    	       	 	//open pipe
		printf("N_pipe2 error\n");                              //if the pipe did not open properly exit
		exit(-1);

	}
	if(pipe(buffer->simple) < 0){						//simple pipe for part 4 
		printf("\nSimple pipe error...");
		exit(-1);
	}
	if((dummy = fork()) < 0){						//fork the program and 
		printf("\nError Forking lab4.c");				//check for successful operation
		exit(-1);
	}	

	if(dummy == 0){								//for process 2 use the child 
		process2(*buffer);
	}

	else{
		pthread_t t0, printing;
		pthread_create(&t0, NULL, (void *)thread0, (void *)buffer);		//create thread to process 
//		pthread_create(&printing, NULL, (void *)print, (void *)buffer);	//create thread to print data to screen 

		if((N_pipe1 = open("/tmp/N_pipe1", O_RDONLY)) < 0){			//open pipe
			printf("N_pipe1 error\n");					//if the pipe did not open properly exit
			exit(-1);
		}

		while(1){								//while loop to read in data from the GPS_device
			if(read(N_pipe1, &number, sizeof(number)) < 0){		//read in pipe and store to buffer 
				printf("\nN_pipe1 reading error\n");
				exit(-1);
			}else {	
				data = number;	
				clock_gettime(CLOCK_MONOTONIC, (buffer->t));
				flag = 1;
				usleep(10000);
			}
		}
		pthread_join(t0, NULL);
		pthread_join(printing, NULL);
	}
	return 0;
}
//thread0 function that prints buffer to the screen to make sure its working properly 
void *thread0(void * num){

	buff *buffer = (buff *)num;
	int dummy;
	//	data *info = malloc(sizeof(data));
	data info;

	printf("\niontializing thread0");
	while(1){
	//	if(read(*(buffer->N_pipe2), &(info->hittime), sizeof(info->hittime)) != sizeof(info->hittime)){
	//		printf("\nError Reading in from N_pipe2 in thread0");
	//		exit(-1);
	//	}

	//	else{
	//		*(buffer->flag) = 0;
			printf("\nButton press detected...");
			info->buffer = buffer;
			info->before = *(buffer->gpsdata);
			info->previous = *(buffer->t);
	//		printf("\nButton pressed at time %d:%ld", info->hittime.tv_sec, info->hittime.tv_nsec);
			printf("\nBefore Data: %d at %d:%ld", info->before, info->previous.tv_sec, info->previous.tv_nsec);	
			usleep(250000);
	//		pthread_t nt;
	//		pthread_create(&nt, NULL, (void *)interpolate, (void *)info);
	//	}
	}




pthread_exit(0);
}

//second process that will check if the buton has been pressed 
void process2(buff buffer){

	pthread_t t1; 								//create thread 
	pthread_create(&t1, NULL, (void *)buttonThread, (void *)&buffer); 	//to check button periodically 
	pthread_join(t1, NULL);							//join thread 

}
//thread function to check wether the button has been pressed 
void *buttonThread(void *ptr){
	printf("\nEntering buton thread...");
	buff buffer = *((buff *)ptr);						//typecast pipe 
	struct timespec c1;							//create a clock to get time with 
	int timer = timerfd_create(CLOCK_MONOTONIC, 0);				//create a timer 
	struct itimerspec itval;

	itval.it_value.tv_sec = 0;						//set the start time of the timer 
	itval.it_value.tv_nsec = 500;
	itval.it_interval.tv_nsec = 0;						//set the period of our timer 
	itval.it_interval.tv_nsec = PERIOD;

	struct sched_param param;						//set up scheduler 
	param.sched_priority = PRIORITY;
	sched_setscheduler(0, SCHED_FIFO, &param);
	uint64_t num_periods = 0;

	timerfd_settime(timer, 0, &itval, NULL);				//set timer and read first value 
	read(timer, &num_periods, sizeof(num_periods));
	while(1){								//enter infinite while loop
		if(check_button() == 1){					//check if button has been pressed 
			sem_wait(buffer.sem2);
			clock_gettime(CLOCK_MONOTONIC, &c1);			//get clocktime so we can send it through the pipe
			if((write(*(buffer.N_pipe2), &c1, sizeof(c1))) < 0){	//write to pipe
				printf("\nError writing to N_pipe2 in ButtonThread function");
				exit(-1);
			}
			sem_post(buffer.sem2);
			usleep(1000);
			clear_button();
		}
		read(timer, &num_periods, sizeof(num_periods));	
	}

	pthread_exit(0);
}
//thread function to interpolate the data 
void *interpolate(void *ptr){
	printf("\nDATA INTERPOLATION");
	int old, new;
	struct timespec told, tnew;
	data info = *((data *)ptr);						//typecast our void pointer so we can access the data 
	long double slope, intercept;
	old = info.before;
	told = info.previous;
	
/*
		while((*(info.buffer->flag) == 0)){}
		
		tnew = *(info.buffer->t);
		new = *(info.buffer->gpsdata);

		printf("\nAfter: %d Before: %d... calculating approximation..", info.after, info.before);

		slope = (double)(((double)info.after - (double)info.before)/((double)info.previous.tv_nsec - (double)info.next.tv_nsec));
		intercept =(double)((double)(info.next.tv_nsec) - (slope*(double)info.after));	
		info.actual = (int)(slope*info.hittime.tv_nsec) + (int)intercept;
		printf("\nSlope: %lf and intercept: %lf, gps data: %d", slope, intercept, info.actual);

*/
	//	write(*(info.simple), &info, sizeof(info));				//simple pipe to write to...

	pthread_exit(0);
}
//thread function to read from simple pipe and write interpolated data to the user 
void *print(void *ptr){
	printf("\nEntered printing thread ");
	buff buffer = *((buff *)ptr);
	data info;

	/*	while(1){				//infinite while loop to read then print the data 
		read(*(buffer.simple), &info, sizeof(info));

		printf("\nPrevious GPS_data: %d at time %lf", info.before, info.previous.tv_nsec);
		printf("\nActual GPS_data: %d at time %lf", info.actual, info.hittime.tv_nsec);
		printf("\nPrevious GPS_data: %d at time %lf", info.before, info.previous.tv_nsec);	
		}*/
	pthread_exit(0);
}

