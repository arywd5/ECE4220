//Allison Yaeger 
//14244528
//Lab2 Week2

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sched.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/timerfd.h>
#include <time.h>
#include <pthread.h>


#define MY_PRIORITY 51
#define PERIOD1 100000
#define WAIT1 500
#define WAIT2 1000
#define WAIT3 1500

//structure to pass to the threads 
typedef struct Arguments{
	FILE *fPtr;	//to point to the file we will scan in from 
	char *buffer;	//common buffer to save the new string to  
	int waittime;
	char **song;
}args;

void *read1( args *ptr);	//this function will read in file and save to the common buffer 
void *read2(args *ptr);		//this function reads in the second file and saves to the common buffer 
void *join(args *ptr);		//this function will join the two files and recreate the song 

int main(){
	
	FILE *ffPtr, *lfPtr;
	//open files for reading 	
	ffPtr = fopen("first.txt", "r");
	lfPtr = fopen("second.txt", "r");
	
	//check that files opened properly 
	if(ffPtr == NULL || lfPtr == NULL ){
		printf("\nOne or more files could not be opened please try again.");
		return -1;
	}
	
	char *buff;				//buffer to pass to the different threads 
	buff = malloc(sizeof(char)*20);
	char **stringArray;
	int i;
	stringArray = (char *)malloc(sizeof(char *) *20)
	for(i = 0; i < 20; i++)			//malloc for size of 20 characters 
		*(stringArray + i) = (char *)malloc(sizeof(char)*20);

	 
	args t1, t2, t3;			//structure to send to the different thread functions
	t1.fPtr = ffPtr;			//point all of our arguments to the correct files and buffer
	t1.buffer = buff;
	t1.waittime = WAIT1;
	t2.fPtr = lfPtr;
	t2.buffer = buff;
	t2.waittime = WAIT2;
	t3.fPtr = NULL;
	t3.buffer = buff;
	t3.waittime = WAIT3;
	t3.song = stringArray; 
	
	//create our threads
	pthread_t thread1, thread2, thread3;
	pthread_create(&thread1, NULL, (void *)read1, (void *)&t1);
	pthread_create(&thread2, NULL, (void *)read1, (void *)&t2);
	pthread_create(&thread3, NULL, (void *)join, (void *)&t3);

	//join threads 
	pthread_join(thread1, NULL);
	pthread_join(thread2, NULL);
	pthread_join(thread3, NULL);	
	
	for(i = 0; i < 20; i++){
		printf("\n%s", *(stringArray + i));
	}

	
	//make sure to close files to avoid a memory leak 
	fclose(ffPtr);
	fclose(lfPtr);
	free(buff);	
	for(i = 0; i < 20; i++)
		free(*(stringArray +i));
	free(stringArray);
		
	return 0;
}

void *read1(args *ptr){
	args *data;
	data = (args *)(ptr);
	
	int timer = timerfd_create(CLOCK_MONOTONIC, 0);
	struct itimerspec itval;

	//set up start time 
	itval.it_value.tv_sec = 0;
	itval.it_value.tv_nsec = data->waittime; 
	
	//set up period 	
	itval.it_interval.tv_sec = 0;
	itval.it_interval.tv_nsec = PERIOD1;
	

	//set the scheduler 
	struct sched_param param;
	param.sched_priority = MY_PRIORITY;
	sched_setscheduler(0, SCHED_FIFO, &param);
	uint64_t num_periods = 0;

	//Start timer 
	timerfd_settime(timer, 0, &itval, NULL);
	
	while(!feof){
	
		read(timer, &num_periods, sizeof(num_periods));
		fscanf(data->fPtr, "%s\n", data->buffer);

	}		
	if(num_periods > 1){
		printf("\nMISSED WINDOW");
		exit(0);
	}

	pthread_exit(0);
}

void *join(args *ptr){
	args *data;
	data = (args *)(ptr);

	//set the scheduler
        struct sched_param param;
        param.sched_priority = MY_PRIORITY;
        sched_setscheduler(0, SCHED_FIFO, &param);
        uint64_t num_periods = 0;

	int timer = timerfd_create(CLOCK_MONOTONIC, 0);
        struct itimerspec itval;

        //set up start time
        itval.it_value.tv_sec = 0;
        itval.it_value.tv_nsec = data->waittime;

        //set up period
        itval.it_interval.tv_sec = 0;
        itval.it_interval.tv_nsec = (PERIOD1/2);
	
	//Start timer
        timerfd_settime(timer, 0, &itval, NULL);
	
	read(timer, &num_periods, sizeof(num_periods));
	*(data->song) = *(data->buffer);  	
	
	if(num_periods > 1){
		printf("\nMISSED WINDOW");
		exit(0);
	}

	pthread_exit(0);
}
